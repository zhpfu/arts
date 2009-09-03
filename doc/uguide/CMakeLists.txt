project (uguide)
find_package (LATEX)

if (PDFLATEX_COMPILER AND BIBTEX_COMPILER)

  configure_file (${CMAKE_CURRENT_SOURCE_DIR}/auto_version.tex.in
                  ${CMAKE_CURRENT_BINARY_DIR}/auto_version.tex @ONLY)

  file (GLOB TEXFILES "${CMAKE_CURRENT_SOURCE_DIR}/*.tex")
  file (GLOB FIGFILES "${CMAKE_CURRENT_SOURCE_DIR}/Figs/*/*.pdf")

  set (PDFLATEX_OPTIONS "-interaction;batchmode;-halt-on-error")
  set (PDFLATEX_LOGGING ">>;uguide.log")
  set (PDFLATEX_COMMAND "TEXINPUTS=.:${CMAKE_CURRENT_SOURCE_DIR}:"
    "save_size=15000" ${PDFLATEX_COMPILER} ${PDFLATEX_OPTIONS} uguide
    ${PDFLATEX_LOGGING})

  add_custom_command (
    OUTPUT uguide.pdf
    COMMAND ${CMAKE_COMMAND} -E remove -f uguide.log
    COMMAND ${PDFLATEX_COMMAND}
            && makeindex -q uguide.idx ${PDFLATEX_LOGGING}
            && BSTINPUTS=.:${CMAKE_CURRENT_SOURCE_DIR}: BIBINPUTS=.:${CMAKE_CURRENT_SOURCE_DIR}: save_size=15000
              ${BIBTEX_COMPILER} uguide ${PDFLATEX_LOGGING}
            && ${PDFLATEX_COMMAND}
            && ${PDFLATEX_COMMAND}
            && ${PDFLATEX_COMMAND} || \(cat uguide.log && exit 1\)
    DEPENDS ${TEXFILES} ${FIGFILES} ${CMAKE_CURRENT_BINARY_DIR}/auto_version.tex
  )

  add_custom_target (uguide ALL DEPENDS
    ${CMAKE_CURRENT_BINARY_DIR}/uguide.pdf)

  set (TEXCLEANFILES "")
  foreach (TEXFILE ${TEXFILES})
    get_filename_component (BASETEXFILE ${TEXFILE} NAME_WE)
    set (TEXCLEANFILES ${TEXCLEANFILES} ${BASETEXFILE}.aux)
  endforeach (TEXFILE)

  set (TEXCLEANFILES ${TEXCLEANFILES}
       uguide.bbl uguide.blg uguide.idx uguide.ilg uguide.ind
       uguide.log uguide.out uguide.toc)

  set_directory_properties (PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES
                            "${TEXCLEANFILES}")

endif (PDFLATEX_COMPILER AND BIBTEX_COMPILER)
