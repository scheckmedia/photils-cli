if(APPLE)
  set(APPS "\${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}.app")
  set(DIRS "\${CMAKE_INSTALL_PREFIX}/share")
  install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION . COMPONENT Runtime
    RUNTIME DESTINATION bin COMPONENT Runtime
  )
  install(DIRECTORY ${RESOURCE_FOLDER}/share/photils/ DESTINATION ${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}.app/Contents/share/photils)

  install(CODE "
    include(BundleUtilities)
    fixup_bundle(\"${APPS}\"  \"\"  \"${DIRS}\")
  " COMPONENT Runtime)
elseif(WIN32)
  # include(InstallRequiredSystemLibraries)
  install(TARGETS ${PROJECT_NAME} DESTINATION bin)
  install(DIRECTORY ${RESOURCE_FOLDER}/share/photils/ DESTINATION share/photils)
  set(CPACK_GENERATOR "ZIP")
  set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
  include(CPack)
else()
  install(TARGETS ${PROJECT_NAME} DESTINATION bin)
  install(FILES appimage_resources/photils-cli.desktop DESTINATION share/applications/)
  install(FILES appimage_resources/photils.svg DESTINATION share/icons/)
  install(FILES appimage_resources/photils-cli.appdata.xml DESTINATION share/metainfo/)
  install(DIRECTORY ${RESOURCE_FOLDER}/share/photils/ DESTINATION ${CMAKE_INSTALL_PREFIX}/share/photils)
endif()
