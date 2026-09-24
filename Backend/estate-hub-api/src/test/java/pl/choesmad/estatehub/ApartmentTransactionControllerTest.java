package pl.choesmad.estatehub;

import org.junit.jupiter.api.Test;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.context.SpringBootTest;
import org.springframework.boot.webmvc.test.autoconfigure.AutoConfigureMockMvc;
import org.springframework.test.web.servlet.MockMvc;
import org.springframework.transaction.annotation.Transactional;
import pl.choesmad.estatehub.domain.Apartment;
import pl.choesmad.estatehub.domain.ApartmentStatus;
import pl.choesmad.estatehub.repository.ApartmentRepository;

import static org.springframework.test.web.servlet.request.MockMvcRequestBuilders.patch;
import static org.springframework.test.web.servlet.result.MockMvcResultMatchers.status;

@SpringBootTest
@AutoConfigureMockMvc
@Transactional
class ApartmentTransactionControllerTest {

    @Autowired
    private MockMvc mockMvc;

    @Autowired
    private ApartmentRepository apartmentRepository;

    @Test
    void reserveAvailableApartmentSucceeds() throws Exception {
        apartmentRepository.saveAndFlush(
                new Apartment("APT_TEST_100", 400000, 2, ApartmentStatus.AVAILABLE, false));

        mockMvc.perform(patch("/api/v1/apartments/APT_TEST_100/reserve"))
                .andExpect(status().isOk());
    }

    @Test
    void reserveAlreadyReservedApartmentFails() throws Exception {
        apartmentRepository.saveAndFlush(
                new Apartment("APT_TEST_101", 400000, 2, ApartmentStatus.RESERVED, false));

        mockMvc.perform(patch("/api/v1/apartments/APT_TEST_101/reserve"))
                .andExpect(status().isConflict());
    }

    @Test
    void reserveNonExistentApartmentReturnsNotFound() throws Exception {
        mockMvc.perform(patch("/api/v1/apartments/APT_DOES_NOT_EXIST/reserve"))
                .andExpect(status().isNotFound());
    }

    @Test
    void purchaseReservedApartmentSucceeds() throws Exception {
        apartmentRepository.saveAndFlush(
                new Apartment("APT_TEST_102", 400000, 2, ApartmentStatus.RESERVED, false));

        mockMvc.perform(patch("/api/v1/apartments/APT_TEST_102/purchase"))
                .andExpect(status().isOk());
    }

    @Test
    void purchaseAvailableApartmentFails() throws Exception {
        apartmentRepository.saveAndFlush(
                new Apartment("APT_TEST_103", 400000, 2, ApartmentStatus.AVAILABLE, false));

        mockMvc.perform(patch("/api/v1/apartments/APT_TEST_103/purchase"))
                .andExpect(status().isConflict());
    }

    @Test
    void releaseReservedApartmentSucceeds() throws Exception {
        apartmentRepository.saveAndFlush(
                new Apartment("APT_TEST_104", 400000, 2, ApartmentStatus.RESERVED, false));

        mockMvc.perform(patch("/api/v1/apartments/APT_TEST_104/release"))
                .andExpect(status().isOk());
    }

    @Test
    void releaseAvailableApartmentFails() throws Exception {
        apartmentRepository.saveAndFlush(
                new Apartment("APT_TEST_105", 400000, 2, ApartmentStatus.AVAILABLE, false));

        mockMvc.perform(patch("/api/v1/apartments/APT_TEST_105/release"))
                .andExpect(status().isConflict());
    }
}