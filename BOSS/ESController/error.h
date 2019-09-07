//Dear emacs, this is -*- c++ -*-

/**
 * @file error.h
 * @author <a href="dosil@ifae.es">Mireia Dosil</a> 
 * $Revision: 1.1.1.1 $
 * $Date: 2007/04/26 12:43:06 $
 *
 * @brief Defines the error and reporting policies
 */

#ifndef EFPSC_ERROR_H
#define EFPSC_ERROR_H

#define EFPSC_ERROR_STREAM 2
#define EFPSC_ERROR_STDOUT 0
#define EFPSC_ERROR_NOERROR 2

#if (EFPSC_ERROR_STREAM==EFPSC_ERROR_STDOUT)
#include <iostream>
#define EFPSC_ERROR(p,m) std::cerr << "[" <<p<< "] ERROR: " << m << std::endl
#define EFPSC_WARN(p,m) std::cerr << "[" <<p<< "] WARNING: "<< m << std::endl
#define EFPSC_INFO(p,m) std::cerr << "[" << p << "] " << m << std::endl
#define EFPSC_DBG1(p,m) std::cout << "[DBG1 " <<p<< "] " << m << std::endl
#define EFPSC_DBG2(p,m) std::cout << "[DBG2 " <<p<< "] " << m << std::endl
#define EFPSC_DBG3(p,m) std::cout << "[DBG3 " <<p<< "] " << m << std::endl
#endif //EFPSC_ERROR_STDOUT



#if (EFPSC_ERROR_STREAM==EFPSC_ERROR_NOERROR)
#define EFPSC_ERROR(p,m)
#define EFPSC_WARN(m)
#define EFPSC_INFO(m)
#define EFPSC_DBG1(p,m)
#define EFPSC_DBG2(p,m)
#define EFPSC_DBG3(p,m)
#endif //EFPSC_ERROR_NOERROR

#endif /* EFPSC_ERROR_H */
