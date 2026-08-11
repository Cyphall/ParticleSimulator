vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO Cyphall/CyphGPU
	REF "333a912e29ff21e78eb71e489d4825aced166721"
	SHA512 b3f67ca927966244685b7d1bfe91b4c5a5d9a5f00a4fa334233c0ed0088d23987b7d81a7df04406db550e8193df93cd82167bf2ba27adbfd89ea74f6691649a8
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
