package pl.choesmad.estatehub;

import org.junit.jupiter.api.Test;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.data.jpa.test.autoconfigure.DataJpaTest;
import org.springframework.boot.jdbc.test.autoconfigure.AutoConfigureTestDatabase;
import pl.choesmad.estatehub.domain.Apartment;
import pl.choesmad.estatehub.domain.ApartmentStatus;
import pl.choesmad.estatehub.repository.ApartmentRepository;

import java.util.Optional;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

@DataJpaTest
@AutoConfigureTestDatabase(replace = AutoConfigureTestDatabase.Replace.NONE)
class ApartmentRepositoryTest {

    @Autowired
    private ApartmentRepository apartmentRepository;

    @Test
    void savesAndReadsApartment() {
        Apartment saved = apartmentRepository.saveAndFlush(
                new Apartment("APT_TEST_001", 500000, 3, ApartmentStatus.AVAILABLE, true)
        );

        Optional<Apartment> found = apartmentRepository.findById(saved.getId());

        assertTrue(found.isPresent());
        assertEquals("APT_TEST_001", found.get().getExternalId());
        assertEquals(500000, found.get().getPrice());
        assertEquals(3, found.get().getRooms());
        assertEquals(ApartmentStatus.AVAILABLE, found.get().getStatus());
        assertTrue(found.get().isHasGarage());
    }
}