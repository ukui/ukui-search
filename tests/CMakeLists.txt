add_executable(runnermodeltest runnermodeltest.cpp ../src/runmodel/runnermodel.cpp seachwidget.cpp filtermodel.cpp)
target_link_libraries(runnermodeltest Qt5::Widgets KF5::Service KF5Runner)
#ecm_mark_as_test(runnermodeltest)
