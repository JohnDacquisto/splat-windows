// Edit these defines for SPLAT release.

#define APP_NAME			"SPLAT"		// OLD NAME: splat_name
#define VERSION_NAME		"1.4.2"	// OLD NAME: splat_version
#define VERSION_DATE		"JUL-2020"

#define VERSION_MAJOR		1
#define VERSION_MINOR		4
#define VERSION_REVISION	2
#define VERSION_BUILD		0


// Uncomment this define to include the Visual Leak Detector header.

#ifdef _DEBUG
//#define VISUAL_LEAK_DETECTOR
#endif


// These defines shouldn't need to be edited.

#define STRINGSIZE2(s) #s
#define STRINGSIZE(s) STRINGSIZE2(s)

#define VER_COMPANY_NAME				"SPLAT"
#define VER_PRODUCTNAME_STR				"SPLAT"
#define VER_COPYRIGHT_STR				"Copyright (C) " VER_COMPANY_NAME

#define VER_PRODUCT_VERSION				VERSION_MAJOR, VERSION_MINOR, VERSION_REVISION, VERSION_BUILD

#define VER_PRODUCT_VERSION_STR			STRINGSIZE(VERSION_MAJOR)        \
										"." STRINGSIZE(VERSION_MINOR)    \
										"." STRINGSIZE(VERSION_REVISION) \
										"." STRINGSIZE(VERSION_BUILD)

#define VER_FILE_DESCRIPTION_STR		VER_PRODUCTNAME_STR " component"

#ifdef _DEBUG
#define VER_VER_DEBUG					VER_FF_DEBUG
#else
#define VER_VER_DEBUG					0
#endif

#define VER_FILEOS						VOS_NT_WINDOWS32
#define VER_FILEFLAGS					VER_VER_DEBUG
#define VER_FILETYPE					VFT_APP

#define VERSION_HEADER					VERSION_NAME
#define MINIDUMP_PATH					"C:\\Temp\\SPLAT_Error_Logs\\" VERSION_NAME "\\"
