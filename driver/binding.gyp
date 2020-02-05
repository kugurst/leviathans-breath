{
  "targets": [
    {
      "target_name": "leviathans_breath",
      "sources": [ "rawhid/hid_LINUX.c", "rawhid/driver.cpp", "lb_node_driver.cpp" ],
      "include_dirs" : [
        "<!(node -e \"require('nan')\")"
      ],
      "cflags" : [
        '-DDRIVER', '-DFIXED_POINTS_USE_NAMESPACE', '-DOS_LINUX' '-Wall'
      ],
      "libraries" : [
        '-lusb'
      ]
    }
  ]
}
