cc_binary(
    name = "roam",
    srcs = glob(["src/roam.cpp"]),
    deps = [
        "roam_lib",
        "roam_tools",
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
