vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO Cyphall/CyphGPU
	REF "28a84db80f213d34727d97e6078382fd0dcb73bf"
	SHA512 d95ac6f00af84379905b2ca2cba391d04354dcc697fb54d6a29a40c2c6aee16bc74553b9d6a21163cb99b246c32a8062bc648ffad6ea48c336ba4ed74cac3eea
	HEAD_REF master
)

vcpkg_check_features(OUT_FEATURE_OPTIONS FEATURE_OPTIONS
	FEATURES
		tracy CYPHGPU_ENABLE_TRACY
		imgui-backend CYPHGPU_ENABLE_IMGUI_BACKEND
)

vcpkg_cmake_configure(
	SOURCE_PATH "${SOURCE_PATH}"
	OPTIONS
		-DCYPHGPU_INSTALL=ON
		${FEATURE_OPTIONS}
)

vcpkg_cmake_install()
vcpkg_copy_pdbs()
vcpkg_cmake_config_fixup(CONFIG_PATH "lib/cmake/CyphGPU")

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/share")

vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE")
