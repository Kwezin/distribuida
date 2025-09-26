Instalar dependencias:

    // Rodar todo esse bloco junto:
    sudo apt update
    sudo apt install -y build-essential autoconf libtool pkg-config cmake \
      libgflags-dev libgtest-dev clang libc++-dev libprotobuf-dev protobuf-compiler \
      libgrpc-dev libgrpc++-dev protobuf-compiler-grpc

    // Rodar todo esse bloco junto
    python3 -m pip install --upgrade pip
    python3 -m pip install grpcio grpcio-tools


1 - Cenario C++ servidor e C++ cliente

    Terminal 1 (servidor c++)
      cd build
      env LD_LIBRARY_PATH=/usr/lib/x86_64-linux-gnu ./servidor_cpp
    
    Terminal 2 (cliente c++)
      cd build
      env LD_LIBRARY_PATH=/usr/lib/x86_64-linux-gnu ./cliente_cpp

2 - Cenario Python servidor e C++ cliente

    Terminal 1 (servidor python)
      python3 server_py.py

    Terminal 2 (cliente c++)
      cd build
      env LD_LIBRARY_PATH=/usr/lib/x86_64-linux-gnu ./cliente_cpp


3 - Cenario C++ servidor e Python cliente

    Terminal 1 (servidor c++)
      cd build
      env LD_LIBRARY_PATH=/usr/lib/x86_64-linux-gnu ./servidor_cpp


    Terminal 2 (cliente python)
      python3 client_py.py

4 - Cenario Python servidor e Python cliente

    Terminal 1 (servidor python)
      python3 server_py.py

    Terminal 2 (cliente python)
      python3 client_py.py


      
