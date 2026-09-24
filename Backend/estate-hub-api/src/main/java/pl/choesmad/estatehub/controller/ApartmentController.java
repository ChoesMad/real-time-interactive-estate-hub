package pl.choesmad.estatehub.controller;

import jakarta.validation.constraints.Positive;
import jakarta.validation.constraints.PositiveOrZero;
import org.springframework.validation.annotation.Validated;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PatchMapping;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;
import pl.choesmad.estatehub.domain.ApartmentStatus;
import pl.choesmad.estatehub.dto.ApartmentResponse;
import pl.choesmad.estatehub.service.ApartmentService;

import java.util.List;

@RestController
@Validated
public class ApartmentController {

    private final ApartmentService apartmentService;

    public ApartmentController(ApartmentService apartmentService) {
        this.apartmentService = apartmentService;
    }

    @GetMapping("/api/v1/apartments")
    public List<ApartmentResponse> getApartments(
            @RequestParam(required = false) @PositiveOrZero Integer minPrice,
            @RequestParam(required = false) @PositiveOrZero Integer maxPrice,
            @RequestParam(required = false) ApartmentStatus status,
            @RequestParam(required = false) Boolean hasGarage,
            @RequestParam(required = false) @Positive Integer rooms
    ) {
        return apartmentService.findFiltered(minPrice, maxPrice, status, hasGarage, rooms).stream()
                .map(ApartmentResponse::from)
                .toList();
    }

    @PatchMapping("/api/v1/apartments/{externalId}/reserve")
    public ApartmentResponse reserveApartment(@PathVariable String externalId) {
        return ApartmentResponse.from(apartmentService.reserve(externalId));
    }

    @PatchMapping("/api/v1/apartments/{externalId}/release")
    public ApartmentResponse releaseApartment(@PathVariable String externalId) {
        return ApartmentResponse.from(apartmentService.release(externalId));
    }

    @PatchMapping("/api/v1/apartments/{externalId}/purchase")
    public ApartmentResponse purchaseApartment(@PathVariable String externalId) {
        return ApartmentResponse.from(apartmentService.purchase(externalId));
    }
}