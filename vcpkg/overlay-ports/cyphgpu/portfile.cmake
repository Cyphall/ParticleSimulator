vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO Cyphall/CyphGPU
	REF "3eb5e2e7ac4c5af11b7550bbfd37e1511dd04603"
	SHA512 85513d560a0d2004c9dc1b5360ac36c9ce6fdbc94152c986244a9bdfc6a04155b11b19e917bec946ed0e4fdc342ce8f5a092849c533ea1c73a1f480a03623e9f
	HEAD_REF master
)

vcpkg_cmake_configure(
	SOURCE_PATH "${SOURCE_PATH}"
	OPTIONS
		-DCYPHGPU_INSTALL=ON
)

vcpkg_cmake_install()
vcpkg_copy_pdbs()
vcpkg_cmake_config_fixup(CONFIG_PATH "lib/cmake/CyphGPU")

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")

vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE")
