set(RUKH_NNUE_NETWORKS
    nn-c288c895ea92.nnue
    nn-37f18f62d772.nnue
)

foreach(network IN LISTS RUKH_NNUE_NETWORKS)
    set(network_path "${CMAKE_CURRENT_BINARY_DIR}/${network}")
    if(NOT EXISTS "${network_path}")
        file(DOWNLOAD
            "https://tests.stockfishchess.org/api/nn/${network}"
            "${network_path}"
            TLS_VERIFY ON
        )
    endif()
endforeach()
