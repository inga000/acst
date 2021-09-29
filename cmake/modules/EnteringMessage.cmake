function(entering_message)
    file (RELATIVE_PATH _relPath "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
    message("Entering ${_relPath}")
endfunction(entering_message)