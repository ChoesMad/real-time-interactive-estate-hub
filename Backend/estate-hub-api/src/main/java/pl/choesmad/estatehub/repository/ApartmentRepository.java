package pl.choesmad.estatehub.repository;

import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.Query;
import org.springframework.data.repository.query.Param;
import pl.choesmad.estatehub.domain.Apartment;
import pl.choesmad.estatehub.domain.ApartmentStatus;
import java.util.Optional;

import java.util.List;

public interface ApartmentRepository extends JpaRepository<Apartment, Long> {

    Optional<Apartment> findByExternalId(String externalId);

    @Query("""
            SELECT a FROM Apartment a
            WHERE (:minPrice IS NULL OR a.price >= :minPrice)
              AND (:maxPrice IS NULL OR a.price <= :maxPrice)
              AND (:status IS NULL OR a.status = :status)
              AND (:hasGarage IS NULL OR a.hasGarage = :hasGarage)
              AND (:rooms IS NULL OR a.rooms = :rooms)
            """)
    List<Apartment> findByFilters(
            @Param("minPrice") Integer minPrice,
            @Param("maxPrice") Integer maxPrice,
            @Param("status") ApartmentStatus status,
            @Param("hasGarage") Boolean hasGarage,
            @Param("rooms") Integer rooms
    );
}