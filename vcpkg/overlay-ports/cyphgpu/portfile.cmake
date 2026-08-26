vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO Cyphall/CyphGPU
	REF "31a5443a493afa02b17d1808a4251bb4d47e9aa5"
	SHA512 9f9ec5c706951fb07b19ab3079f2aced08dd09ba4fb04337f00cc0990c0d49a2972871282cb951b2614dc3e1a03b35f1502042b0b5b47cca3ba486591fcef571
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
