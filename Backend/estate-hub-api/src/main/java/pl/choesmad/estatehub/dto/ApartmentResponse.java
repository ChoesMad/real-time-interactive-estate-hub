package pl.choesmad.estatehub.dto;

import pl.choesmad.estatehub.domain.Apartment;
import pl.choesmad.estatehub.domain.ApartmentStatus;

public record ApartmentResponse(
        String externalId,
        int price,
        int rooms,
        ApartmentStatus status,
        boolean hasGarage
) {
    public static ApartmentResponse from(Apartment apartment) {
        return new ApartmentResponse(
                apartment.getExternalId(),
                apartment.getPrice(),
                apartment.getRooms(),
                apartment.getStatus(),
                apartment.isHasGarage()
        );
    }
}