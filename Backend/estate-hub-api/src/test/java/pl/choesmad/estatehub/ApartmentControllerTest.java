package pl.choesmad.estatehub.controller;

import org.junit.jupiter.api.Test;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.context.SpringBootTest;
import org.springframework.boot.webmvc.test.autoconfigure.AutoConfigureMockMvc;
import org.springframework.test.web.servlet.MockMvc;
import org.springframework.test.web.servlet.MvcResult;
import org.springframework.transaction.annotation.Transactional;
import pl.choesmad.estatehub.domain.Apartment;
import pl.choesmad.estatehub.domain.ApartmentStatus;
import pl.choesmad.estatehub.repository.ApartmentRepository;

import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.springframework.test.web.servlet.request.MockMvcRequestBuilders.get;
import static org.springframework.test.web.servlet.result.MockMvcResultMatchers.status;

@SpringBootTest
@AutoConfigureMockMvc
@Transactional
class ApartmentControllerTest {

    @Autowired
    private MockMvc mockMvc;

    @Autowired
    private ApartmentRepository apartmentRepository;

    @Test
    void getApartmentsReturnsSavedApartment() throws Exception {
        apartmentRepository.saveAndFlush(
                new Apartment("APT_TEST_003", 600000, 4, ApartmentStatus.RESERVED, true)
        );

        MvcResult result = mockMvc.perform(get("/api/v1/apartments"))
                .andExpect(status().isOk())
                .andReturn();

        String responseBody = result.getResponse().getContentAsString();
        assertTrue(responseBody.contains("APT_TEST_003"));
    }

    @Test
    void getApartmentsFiltersByStatus() throws Exception {
        apartmentRepository.saveAndFlush(
                new Apartment("APT_TEST_004", 300000, 2, ApartmentStatus.SOLD, false));
        apartmentRepository.saveAndFlush(
                new Apartment("APT_TEST_005", 300000, 2, ApartmentStatus.AVAILABLE, false));

        MvcResult result = mockMvc.perform(get("/api/v1/apartments").param("status", "SOLD"))
                .andExpect(status().isOk())
                .andReturn();

        String responseBody = result.getResponse().getContentAsString();
        assertTrue(responseBody.contains("APT_TEST_004"));
        assertFalse(responseBody.contains("APT_TEST_005"));
    }

    @Test
    void getApartmentsRejectsNegativeMinPrice() throws Exception {
        mockMvc.perform(get("/api/v1/apartments").param("minPrice", "-1"))
                .andExpect(status().isBadRequest());
    }

    @Test
    void getApartmentsRejectsInvalidStatus() throws Exception {
        mockMvc.perform(get("/api/v1/apartments").param("status", "NIEISTNIEJACY"))
                .andExpect(status().isBadRequest());
    }

    @Test
    void getApartmentsRejectsZeroRooms() throws Exception {
        mockMvc.perform(get("/api/v1/apartments").param("rooms", "0"))
                .andExpect(status().isBadRequest());
    }
}