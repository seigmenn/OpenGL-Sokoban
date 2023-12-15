//
// Created by auror on 15/12/2023.
//

#ifndef EXAMAUTUMN2023_FINALAPPLICATION_H
#define EXAMAUTUMN2023_FINALAPPLICATION_H

#include <string>
#include "/framework/GLFWAppliction/GLFWApplication.h"


class FinalApplication : public GLFWApplication
{
public:
    FinalApplication();
    FinalApplication(const std::string& name, const std::string& version);
    ~FinalApplication();

    //Initialization
    virtual unsigned int Init();
    // Run function
    virtual unsigned int Run() const override;
};

#endif //EXAMAUTUMN2023_FINALAPPLICATION_H
