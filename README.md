# Real-Time Interactive Estate Hub

An interactive 3D residential estate visualization (Unreal Engine 5) powered by real-time data from a REST backend 
(Spring Boot + PostgreSQL). The player filters available apartments, clicks directly on a building in the 3D world, 
reserves and purchases selected units — and the state of the entire scene (window colors across all 35 buildings) 
updates live without a refresh.

---

## What This Project Does

- The REST backend exposes data for 506 apartments (price, room count, garage, status) with filtering and validation.
- The Unreal Engine client fetches this data asynchronously on startup and recolors live the windows of each of the 35 physically placed buildings in the scene based on apartment status (available / reserved / sold).
- The player opens a filter panel (price, room count, garage) — only buildings matching the current filter are colored and clickable.
- Clicking a matching building displays a details panel showing price, rooms, garage, and actions: Reserve → Purchase, backed by server-side state validation (you cannot purchase an unreserved unit or reserve an occupied one).
- Purchasing changes the status in the database to SOLD and instantly refreshes the building color in the scene — with no manual refresh or restart needed.

## Architecture

```
┌─────────────────────┐        HTTP / JSON        ┌──────────────────────────┐
│   PostgreSQL        │◄─────────────────────────►│  Spring Boot 4 (Java 25) │
│  (apartment table)  │      Spring Data JPA      │   REST API /api/v1/...   │
└─────────────────────┘                           └────────────┬─────────────┘
                                                               │ GET/PATCH (JSON)
                                                               ▼
                                     ┌─────────────────────────────────────────┐
                                     │  UEstateApiSubsystem (GameInstance)     │
                                     │  — single source of truth in UE         │
                                     │  — HTTP, JSON parsing, filtering,       │
                                     │    reservations, player balance         │
                                     └───────────────┬─────────────────────────┘
                                         event-driven (delegates, no Tick())
                              ┌──────────────────────┼──────────────────────────┐
                              ▼                      ▼                          ▼
                    AApartmentActor ×35      ApartmentFilterWidget      ApartmentDetailsWidget
                    (live material color,     (filter panel, UMG)        (details panel, UMG)
                     event-driven)
```

**Data Flow:** The backend is the single source of truth for apartment states. Unreal Engine never modifies state locally without server confirmation — every action (reservation / cancellation / purchase) goes to the backend first, and only upon success does it update the local cache and emit an event that all listening components react to.

## Key Design Decisions

Kilka nietrywialnych problemów napotkanych i rozwiązanych podczas budowy:

**Merged Mesh Geometry Issue.** The 35 buildings are single, merged (`Merge Actors`) meshes — their world placement is baked directly into the mesh geometry rather than the actor's Transform (all 35 actors share nearly identical Transforms). This ruled out adding indicator components with offsets, as every component landed in the same world location. Solution: Recoloring the existing material slot (`Polychromatic_glass_10`) using `UMaterialInstanceDynamic` instead of spawning new geometry — zero reliance on actor Transforms.

**Event-driven, zero `Tick()`.** All communication across UI ↔ Data ↔ Visualization relies on dynamic delegates (`OnApartmentsLoaded`, `OnFilterChanged`). Each of the 35 actors subscribes once in `BeginPlay` and reacts strictly to actual events (data loaded, filter changed, reservation/purchase confirmed) — no polling, no per-frame recalculations.

**Client-Side Filtering over Server-Side.** While the REST API supports query parameters (useful for generic external clients), the interactive UI sliders filter data locally inside the subsystem. This avoids firing dozens of HTTP requests per second while dragging UI sliders.

**Backend-Enforced State Machine.** Apartment statuses (`AVAILABLE → RESERVED → SOLD`) and valid state transitions are strictly governed on the server side (`409 Conflict` for illegal transitions). The client never speculatively updates local visuals; it waits for backend confirmation first, eliminating race conditions like two players attempting to buy the same unit.

**Backend Layer Separation.** Traditional Controller → Service → Repository pattern with JPA Entities strictly decoupled from DTOs, request validation via `jakarta.validation`, and explicit, tested error mapping to HTTP status codes (400/404/409).

## Tech Stack

| Layer | Technology |
|---|---|
| Backend | Java 25, Spring Boot 4.1.1, Spring Data JPA, Gradle (Kotlin DSL) |
| Database | PostgreSQL |
| 3D Client | Unreal Engine 5.8, C++, Enhanced Input, UMG |
| Backend Testing | JUnit 5, Spring Boot Test (`@DataJpaTest`, `MockMvc`) |

## Struktura repozytorium

```
Backend/estate-hub-api/       — Spring Boot API
  src/main/java/pl/choesmad/estatehub/
    controller/                — REST layer + error handling
    service/                   — business logic
    repository/                — Spring Data JPA
    domain/                    — entities
    dto/                       — response contracts

Source/RealTimeInteractive/   — UE C++ client code
  Public/ Private/
    EstateApiSubsystem         — HTTP communication, data state, filter, reservations
    ApartmentActor              — scene representation of an apartment, status coloring
    ApartmentIdMapping          — central mapping of business ID ↔ asset name
    EstateHudPlayerController   — input handling, UI management
    ApartmentFilterWidget       — filter panel
    ApartmentDetailsWidget      — apartment details panel

Content/                       — Unreal assets (meshes, materials, Widget Blueprints)
```

## How to Run

### Backend
```bash
cd Backend/estate-hub-api
# configure local PostgreSQL database, create 'estate_hub' DB
./gradlew bootRun
```
The API starts at `http://localhost:8080`.

### Unreal Engine
1. Open RealTimeInteractive.uproject.
2. On first run: right-click .uproject → Generate Visual Studio project files, build in Visual Studio.
3. Run Play In Editor — make sure the backend is already running.
4. Press G to open the filter panel.

## API Endpoints

| Metoda | Endpoint | Opis |
|---|---|---|
| `GET` | `/api/v1/apartments` | Apartment listing, optional filters: `minPrice`, `maxPrice`, `status`, `hasGarage`, `rooms` |
| `PATCH` | `/api/v1/apartments/{externalId}/reserve` | Reservation (requires status `AVAILABLE`) |
| `PATCH` | `/api/v1/apartments/{externalId}/release` | Cancel reservation (requires status `RESERVED`) |
| `PATCH` | `/api/v1/apartments/{externalId}/purchase` | Purchase (requires status `RESERVED`) |

Validation errors and state conflicts are returned as `400`/`404`/`409` with body `{"error": "..."}`.

## Testing

The backend is covered by multi-layer integration tests: repository (`@DataJpaTest` on a real database), service (`@SpringBootTest`), and REST (`MockMvc`) — including validation for all state machine transitions (`reserve`/`release`/`purchase` across valid and invalid input statuses).

```bash
cd Backend/estate-hub-api
./gradlew test
```
