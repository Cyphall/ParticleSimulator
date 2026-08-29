vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO Cyphall/CyphGPU
	REF "d5883fa6b35c6f5691708151be7474c98e86de3a"
	SHA512 dd4c5d75a0f430200f7f74f06ae4168c232906130c84de2e7605ebc4ed5cce8d9d8a049d012231d5e08998453696cf2b5d326912685101ff80bb8055f213d0a6
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
