vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO Auburn/FastNoise2
    REF master
    SHA512 7e3f513c58eee287630c6f063fd12de91cd6c975928eb84d298e5bc692ba103d5681b2c09725a9622c053b8eb6fe47a917f48b78382441485a2a7c298b4ec86d
)

vcpkg_cmake_configure(
    SOURCE_PATH ${SOURCE_PATH}
    OPTIONS
        -DFETCHCONTENT_FULLY_DISCONNECTED=OFF
        -DFASTNOISE2_TOOLS=OFF
)

vcpkg_cmake_install()

vcpkg_cmake_config_fixup(
    CONFIG_PATH lib/cmake/FastNoise2
    PACKAGE_NAME FastNoise2
)

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")

vcpkg_install_copyright(
    FILE_LIST "${SOURCE_PATH}/LICENSE"
)
