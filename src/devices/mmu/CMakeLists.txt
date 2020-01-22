add_library(mmu STATIC mmu_E64_II.cpp)

target_link_libraries(mmu kernel)