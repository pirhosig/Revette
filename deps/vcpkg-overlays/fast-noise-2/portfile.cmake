vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO Auburn/FastNoise2
    REF ba93f17
    SHA512 0eb68c366d04348b66b74cbd62dabb2b70ff7582997c2edb028c830ea15e686cfd7b1e8177577dd2b81c420a02daa443bfe7ba0b9f72f21b5ce290316e523a28
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
