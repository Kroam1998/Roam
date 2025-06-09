cc_binary(
    name = "roam",
    srcs = glob(["src/roam.cpp"]),
    deps = [
        "roam_lib",
        "roam_tools",
        "roam_cli",
        "@fmt//:fmt",
        "roam_config",
        "roam_theme",
    ],
)

cc_library(
    name = "roam_lib",
    hdrs = ["src/roam.h"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "roam_tools",
    srcs = ["src/roam_tools.cpp"],
    hdrs = ["src/roam_tools.h",
            "src/roam_shared.h"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "roam_cli",
    srcs = ["src/roam_cli.cpp"],
    hdrs = ["src/roam_cli.h",
            "src/roam_shared.h"],
    deps = ["@fmt//:fmt"],
    visibility = ["//visibility:public"],          
)

cc_library(
    name = "roam_config",
    srcs = ["src/roam_config.cpp"],
    hdrs = ["src/roam_config.h",
            "src/roam_shared.h",
            "src/roam_tools.h"],
    deps = ["@fmt//:fmt"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "roam_theme",
    srcs = ["src/roam_theme.cpp"],
    hdrs = ["src/roam_theme.h",
            "src/roam_tools.h"],
    visibility = ["//visibility:public"],
)
