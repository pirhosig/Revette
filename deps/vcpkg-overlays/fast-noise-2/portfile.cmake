vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO Auburn/FastNoise2
    REF eddaeec
    SHA512 6dc79896c2ba448cdc3ab32bff9197ffdee55892c25a2c318cb3358241a5d68addc50a4a3d66c52c28314f292542c156dafc77573f0c17aac95238fc710d343b
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
