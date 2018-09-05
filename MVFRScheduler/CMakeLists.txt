cmake_minimum_required(VERSION 3.2)
project(MVFRScheduler)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/modules/")
find_package(GUROBI REQUIRED)
include_directories(${GUROBI_INCLUDE_DIR})

set(CMAKE_C_COMPILER /usr/bin/gcc-4.8)
set(CMAKE_CXX_COMPILER /usr/bin/g++-4.8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
        Analysis.cpp
        Analysis.h
        BestFitColorScheduler.cpp
        BestFitColorScheduler.h
        BPosition.h
        CFrame.h
        ChannelAssigner.cpp
        ChannelAssigner.h
        ChannelScheduler.cpp
        ChannelScheduler.h
        Chromosome.h
        ConfigExporter.cpp
        ConfigExporter.h
        DataStructure.cpp
        DataStructure.h
        ExporterParameters.h
        FFCIncrementalScheduler.cpp
        FFCIncrementalScheduler.h
        FirstFitColorScheduler.cpp
        FirstFitColorScheduler.h
        FirstFitParameters.h
        FirstFitScheduler.cpp
        FirstFitScheduler.h
        FirstFitVariantScheduler.cpp
        FirstFitVariantScheduler.h
        Frame.h
        GeneticParameters.h
        GeneticScheduler.cpp
        GeneticScheduler.h
        IGraphColoring.cpp
        IGraphColoring.h
        ILPChanneling.cpp
        ILPChanneling.h
        IndependentParameters.h
        IndependentScheduler.cpp
        IndependentScheduler.h
        IndependentSet.cpp
        IndependentSet.h
        io.cpp
        io.h
        LowerBound.cpp
        LowerBound.h
        MergeSort.cpp
        MergeSort.h
        Message.h
        MVFRScheduler.cpp
        MVFRScheduler.h
        Position.h
        ScheduleChecker.cpp
        ScheduleChecker.h
        simple_svg_1.0.0.cpp
        StageParameters.h
        StageScheduler.cpp
        StageScheduler.h
        Utility.cpp
        Utility.h
        VariantFrame.h
        GanttChartDrawer.cpp
        GanttChartDrawer.h
        simple_svg_1.0.0.h
        FibexExporter.cpp
        FibexExporter.h
        ExporterStructure.h
        ExporterStructure.cpp
        Buffer.h
        NetworkParametersCalculator.cpp
        NetworkParametersCalculator.h
        ILPChannelingGurobi.cpp
        ILPChannelingGurobi.h
        ChannelGeneticAssigner.cpp
        ChannelGeneticAssigner.h
        IterativeChannelScheduler.cpp
        IterativeChannelScheduler.h ChannelSchedulerTests.cpp ChannelSchedulerTests.h ChannelFaultScheduler.cpp ChannelFaultScheduler.h ChannelSchedulerResult.cpp ChannelSchedulerResult.h IndependentSetGurobi.cpp IndependentSetGurobi.h)

include_directories(/usr/include)

add_executable(MVFRScheduler ${SOURCE_FILES})
target_link_libraries(MVFRScheduler ${GUROBI_LIBRARIES} glpk)

add_custom_command(TARGET MVFRScheduler POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${PROJECT_SOURCE_DIR}/csr10.svgf"
        $<TARGET_FILE_DIR:MVFRScheduler>)