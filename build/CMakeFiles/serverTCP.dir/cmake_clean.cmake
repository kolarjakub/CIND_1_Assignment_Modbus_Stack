file(REMOVE_RECURSE
  "serverTCP"
  "serverTCP.pdb"
)

# Per-language clean rules from dependency scanning.
foreach(lang C)
  include(CMakeFiles/serverTCP.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
