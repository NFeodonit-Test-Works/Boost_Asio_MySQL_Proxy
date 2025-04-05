# Boost.Asio proxy for MySQL

The test work on the use of the Boost.Asio library.

## Build

### On Linux

The building is tested on Ubuntu 18.04.

CMake should be 3.4+ versions for LibCMaker.

Use the following commands:

```
git clone https://github.com/NFeodonit-Test-Works/Boost_Asio_MySQL_Proxy.git
cd Boost_Asio_MySQL_Proxy
mkdir build
cd build

cmake .. \
  -DCMAKE_INSTALL_PREFIX:PATH=/full/path/to/project/libraries/install/directory \
  -DCMAKE_BUILD_TYPE:STRING=Release

cmake --build .
```

```CMAKE_INSTALL_PREFIX``` is used for the Boost installation at the stage of the configuring of the main project by CMake.

```CMAKE_BUILD_TYPE``` must be set to ```Release``` or ```Debug``` for the Boost library building with LibCMaker_Boost.


### On Windows

The building is NOT tested on Windows.

CMake should be 3.4+ versions for LibCMaker.

Should work with the following commands:

```
git clone https://github.com/NFeodonit-Test-Works/Boost_Asio_MySQL_Proxy.git
cd Boost_Asio_MySQL_Proxy
mkdir build
cd build

set prj_BUILD_TYPE=Release

cmake .. ^
  -DCMAKE_INSTALL_PREFIX:PATH=\full\path\to\project\libraries\install\directory ^
  -DCMAKE_BUILD_TYPE:STRING=%prj_BUILD_TYPE% ^
  -DBUILD_SHARED_LIBS:BOOL=ON ^
  -Dcmr_USE_MSVC_STATIC_RUNTIME=OFF ^
  -DCMAKE_GENERATOR:STRING="Visual Studio 15 2017" ^
  -DCMAKE_GENERATOR_PLATFORM:STRING="x64" ^
  -DCMAKE_GENERATOR_TOOLSET:STRING="v141,host=x64" ^
  -DCMAKE_CONFIGURATION_TYPES:STRING=%prj_BUILD_TYPE%

cmake --build . --config %prj_BUILD_TYPE%
```

```CMAKE_INSTALL_PREFIX``` is used for the Boost installation at the stage of the configuring of the main project by CMake.

```prj_BUILD_TYPE``` must be set to ```Release``` or ```Debug``` for the Boost library building with LibCMaker_Boost. This variable can have any name.


### Some notes about building

At the stage of the configuring of the main project by CMake, the following steps are performed:

1. The Boost library sources are downloading from the official library sources.

2. Boost library is compiling.

3. Boost library is installing to the pass in ```CMAKE_INSTALL_PREFIX```.

4. Boost library is finding by the CMake command ```find_package(Boost)``` in the main CMake project.

As a result, the configuration phase takes the considerable time.

For more info about the building of the project with LibCMaker_Boost see [LibCMaker project](https://github.com/LibCMaker/LibCMaker).


## Running

Usage:
```
boost-asio-mysql-proxy <client ip> <port> <mysql server ip> <port> <log file>
```


## Testing

The project running has been tested only on Ubuntu 18.04.

The following commands can be used for testing.

In mysql client, run:

```
create database sysbench;
```

And set up access rights to the database for the user ```username``` (must be a valid user name).

In one terminal window:
```
./boost-asio-mysql-proxy 127.0.0.1 16530 127.0.0.1 3306 sql_log.log
```

In another terminal window:
```
THREADS=100
TIME=300
RATE=0
REPORT_INTERVAL=10
DRIVER=mysql
HOST=127.0.0.1
PORT=16530
DB=sysbench
USER=username
PASSWD=userpass
SCRIPT=/usr/share/sysbench/oltp_read_write.lua
ENGINE=innodb
TABLES=10
TABLE_SIZE=100000
SKIP_TRX=off

sysbench \
--threads=${THREADS} --time=${TIME} --rate=${RATE} --report-interval=${REPORT_INTERVAL} \
--db-driver=${DRIVER} --mysql-host=${HOST} --mysql-port=${PORT} \
--mysql-db=${DB} --mysql-user=${USER} --mysql-password=${PASSWD} \
${SCRIPT} --mysql_storage_engine=${ENGINE} --tables=${TABLES} --table_size=${TABLE_SIZE} --skip_trx=${SKIP_TRX} \
prepare

sysbench \
--threads=${THREADS} --time=${TIME} --rate=${RATE} --report-interval=${REPORT_INTERVAL} \
--db-driver=${DRIVER} --mysql-host=${HOST} --mysql-port=${PORT} \
--mysql-db=${DB} --mysql-user=${USER} --mysql-password=${PASSWD} \
${SCRIPT} --mysql_storage_engine=${ENGINE} --tables=${TABLES} --table_size=${TABLE_SIZE} --skip_trx=${SKIP_TRX} \
run

sysbench \
--threads=${THREADS} --time=${TIME} --rate=${RATE} --report-interval=${REPORT_INTERVAL} \
--db-driver=${DRIVER} --mysql-host=${HOST} --mysql-port=${PORT} \
--mysql-db=${DB} --mysql-user=${USER} --mysql-password=${PASSWD} \
${SCRIPT} --mysql_storage_engine=${ENGINE} --tables=${TABLES} --table_size=${TABLE_SIZE} --skip_trx=${SKIP_TRX} \
cleanup
```

All SQL requests from the client to the MySQL server should be in the file ```sql_log.log```.


## Used documentation

- [Boost.Asio](https://www.boost.org/doc/libs/1_69_0/doc/html/boost_asio.html)
- [MySQL Source Code Documentation](https://dev.mysql.com/doc/dev/mysql-server/latest/)
  - [MySQL Packets](https://dev.mysql.com/doc/dev/mysql-server/latest/page_protocol_basic_packets.html)
  - [Connection Lifecycle](https://dev.mysql.com/doc/dev/mysql-server/latest/page_protocol_connection_lifecycle.html)
