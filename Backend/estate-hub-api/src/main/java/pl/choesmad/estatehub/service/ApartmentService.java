package pl.choesmad.estatehub.service;

import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;
import pl.choesmad.estatehub.domain.Apartment;
import pl.choesmad.estatehub.domain.ApartmentStatus;
import pl.choesmad.estatehub.exception.ApartmentNotFoundException;
import pl.choesmad.estatehub.exception.InvalidApartmentStateException;
import pl.choesmad.estatehub.repository.ApartmentRepository;

import java.util.List;

@Service
public class ApartmentService {

    private final ApartmentRepository apartmentRepository;

    public ApartmentService(ApartmentRepository apartmentRepository) {
        this.apartmentRepository = apartmentRepository;
    }

    public List<Apartment> findAll() {
        return apartmentRepository.findAll();
    }

    public List<Apartment> findFiltered(Integer minPrice, Integer maxPrice, ApartmentStatus status,
                                        Boolean hasGarage, Integer rooms) {
        return apartmentRepository.findByFilters(minPrice, maxPrice, status, hasGarage, rooms);
    }

    @Transactional
    public Apartment reserve(String externalId) {
        Apartment apartment = findByExternalIdOrThrow(externalId);

        if (apartment.getStatus() != ApartmentStatus.AVAILABLE) {
            throw new InvalidApartmentStateException(
                    "Apartment " + externalId + " is not AVAILABLE (current status: " + apartment.getStatus() + ")");
        }

        apartment.setStatus(ApartmentStatus.RESERVED);
        return apartmentRepository.save(apartment);
    }

    @Transactional
    public Apartment release(String externalId) {
        Apartment apartment = findByExternalIdOrThrow(externalId);

        if (apartment.getStatus() != ApartmentStatus.RESERVED) {
            throw new InvalidApartmentStateException(
                    "Apartment " + externalId + " is not RESERVED (current status: " + apartment.getStatus() + ")");
        }

        apartment.setStatus(ApartmentStatus.AVAILABLE);
        return apartmentRepository.save(apartment);
    }

    @Transactional
    public Apartment purchase(String externalId) {
        Apartment apartment = findByExternalIdOrThrow(externalId);

        if (apartment.getStatus() != ApartmentStatus.RESERVED) {
            throw new InvalidApartmentStateException(
                    "Apartment " + externalId + " is not RESERVED (current status: " + apartment.getStatus() + ")");
        }

        apartment.setStatus(ApartmentStatus.SOLD);
        return apartmentRepository.save(apartment);
    }

    private Apartment findByExternalIdOrThrow(String externalId) {
        return apartmentRepository.findByExternalId(externalId)
                .orElseThrow(() -> new ApartmentNotFoundException("Apartment not found: " + externalId));
    }
}