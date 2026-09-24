package pl.choesmad.estatehub.domain;

import jakarta.persistence.Column;
import jakarta.persistence.Entity;
import jakarta.persistence.EnumType;
import jakarta.persistence.Enumerated;
import jakarta.persistence.GeneratedValue;
import jakarta.persistence.GenerationType;
import jakarta.persistence.Id;
import jakarta.persistence.Table;

@Entity
@Table(name = "apartment")
public class Apartment {

    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    @Column(name = "id")
    private Long id;

    @Column(name = "external_id", nullable = false, unique = true)
    private String externalId;

    @Column(name = "price", nullable = false)
    private int price;

    @Column(name = "rooms", nullable = false)
    private int rooms;

    @Enumerated(EnumType.STRING)
    @Column(name = "status", nullable = false)
    private ApartmentStatus status;

    @Column(name = "has_garage", nullable = false)
    private boolean hasGarage;

    protected Apartment() { }

    public Apartment(String externalId, int price, int rooms, ApartmentStatus status, boolean hasGarage) {
        this.externalId = externalId;
        this.price = price;
        this.rooms = rooms;
        this.status = status;
        this.hasGarage = hasGarage;
    }

    public Long getId() {
        return id;
    }

    public String getExternalId() {
        return externalId;
    }

    public int getPrice() {
        return price;
    }

    public int getRooms() {
        return rooms;
    }

    public ApartmentStatus getStatus() {
        return status;
    }

    public void setStatus(ApartmentStatus status) {
        this.status = status;
    }

    public boolean isHasGarage() {
        return hasGarage;
    }
}