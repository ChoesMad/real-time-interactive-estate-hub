package pl.choesmad.estatehub.service;

import org.junit.jupiter.api.Test;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.context.SpringBootTest;
import org.springframework.transaction.annotation.Transactional;
import pl.choesmad.estatehub.domain.Apartment;
import pl.choesmad.estatehub.domain.ApartmentStatus;
import pl.choesmad.estatehub.repository.ApartmentRepository;

import java.util.List;

import static org.junit.jupiter.api.Assertions.assertTrue;

@SpringBootTest
@Transactional
class ApartmentServiceTest {

    @Autowired
    private ApartmentService apartmentService;

    @Autowired
    private ApartmentRepository apartmentRepository;

    @Test
    void findAllReturnsSavedApartment() {
        apartmentRepository.saveAndFlush(
                new Apartment("APT_TEST_002", 450000, 2, ApartmentStatus.AVAILABLE, false)
        );

        List<Apartment> result = apartmentService.findAll();

        assertTrue(result.stream()
                .anyMatch(apartment -> "APT_TEST_002".equals(apartment.getExternalId())));
    }
}