{
  "targets": [
    {
      "target_name": "leviathans_breath",
      "sources": [ "lb_node_driver.cpp", "rawhid/hid_LINUX.c", "rawhid/driver.cpp" ],
      "include_dirs" : [
        "<!(node -e \"require('nan')\")"
      ],
      "cflags" : [
        '-DDRIVER', '-DFIXED_POINTS_USE_NAMESPACE', '-DOS_LINUX', '-Wall'  # , '-std=c++11'
      ],
      "libraries" : [
        '-lusb'
      ],
    }
  ]
}
