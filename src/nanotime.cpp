#include <iostream>
#include <functional>
#include <Rcpp.h>
#include "globals.hpp"
#include "utilities.hpp"
#include "pseudovector.hpp"


typedef ConstPseudoVector<REALSXP, double>   ConstPseudoVectorInt64;
typedef ConstPseudoVector<STRSXP,  const Rcpp::CharacterVector::const_Proxy> ConstPseudoVectorChar;


static inline Global::duration getOffsetCnv(const Global::dtime& dt, const std::string& z) {
  typedef int GET_OFFSET_FUN(long long, const char*); 
  GET_OFFSET_FUN *getOffset = (GET_OFFSET_FUN *) R_GetCCallable("RcppCCTZ", "_RcppCCTZ_getOffset" );

  auto offset = getOffset(std::chrono::duration_cast<std::chrono::seconds>(dt.time_since_epoch()).count(), z.c_str());
  return Global::duration(offset).count() * std::chrono::seconds(1);
}


RcppExport SEXP _nanotime_wday(SEXP tm_p, SEXP tz_p) {
  try {
    checkVectorsLengths(tm_p, tz_p);
    const Rcpp::NumericVector   tm_v(tm_p);
    const Rcpp::CharacterVector tz_v(tz_p);
    ConstPseudoVectorInt64 tm(tm_v);
    ConstPseudoVectorChar  tz(tz_v);
    Rcpp::IntegerVector    res(tm_v.size());

    for (R_xlen_t i=0; i<tm_v.size(); ++i) {
      const auto tz_i = Rcpp::as<std::string>(tz[i]);
      const auto tm_i = *reinterpret_cast<const Global::dtime*>(&tm[i]);
      const auto offset = getOffsetCnv(tm_i, tz_i.c_str());
      date::sys_days t_days = date::floor<date::days>(tm_i + offset);
      res[i] = unsigned(date::weekday(t_days));
    }
    copyNames(tm_v, tz_v, res);    
    return res;
  } catch(std::exception &ex) {	
    forward_exception_to_r(ex);
  } catch(...) { 
    ::Rf_error("c++ exception (unknown reason)"); 
  }

  return 0;                     // not reached
}

RcppExport SEXP _nanotime_mday(SEXP tm_p, SEXP tz_p) {
  try {
    checkVectorsLengths(tm_p, tz_p);
    const Rcpp::NumericVector   tm_v(tm_p);
    const Rcpp::CharacterVector tz_v(tz_p);
    ConstPseudoVectorInt64 tm(tm_v);
    ConstPseudoVectorChar  tz(tz_v);
    Rcpp::IntegerVector    res(tm_v.size());

    for (R_xlen_t i=0; i<tm_v.size(); ++i) {
      const auto tz_i = Rcpp::as<std::string>(tz[i]);
      const auto tm_i = *reinterpret_cast<const Global::dtime*>(&tm[i]);
      const auto offset = getOffsetCnv(tm_i, tz_i.c_str());
      date::sys_days t_days = date::floor<date::days>(tm_i + offset);
      date::weekday wd(t_days);
      res[i] = unsigned(date::year_month_day(t_days).day());
    }
    copyNames(tm_v, tz_v, res);    
    return res;
  } catch(std::exception &ex) {	
    forward_exception_to_r(ex);
  } catch(...) { 
    ::Rf_error("c++ exception (unknown reason)"); 
  }

  return 0;                     // not reached
}

RcppExport SEXP _nanotime_month(SEXP tm_p, SEXP tz_p) {
  try {
    checkVectorsLengths(tm_p, tz_p);
    const Rcpp::NumericVector   tm_v(tm_p);
    const Rcpp::CharacterVector tz_v(tz_p);
    ConstPseudoVectorInt64 tm(tm_v);
    ConstPseudoVectorChar  tz(tz_v);
    Rcpp::IntegerVector    res(tm_v.size());

    for (R_xlen_t i=0; i<tm_v.size(); ++i) {
      const auto tz_i = Rcpp::as<std::string>(tz[i]);
      const auto tm_i = *reinterpret_cast<const Global::dtime*>(&tm[i]);
      const auto offset = getOffsetCnv(tm_i, tz_i.c_str());
      date::sys_days t_days = date::floor<date::days>(tm_i + offset);
      date::weekday wd(t_days);
      res[i] = unsigned(date::year_month_day(t_days).month());
    }
    copyNames(tm_v, tz_v, res);    
    return res;
  } catch(std::exception &ex) {	
    forward_exception_to_r(ex);
  } catch(...) { 
    ::Rf_error("c++ exception (unknown reason)"); 
  }

  return 0;                     // not reached
}

RcppExport SEXP _nanotime_year(SEXP tm_p, SEXP tz_p) {
  try {
    checkVectorsLengths(tm_p, tz_p);
    const Rcpp::NumericVector   tm_v(tm_p);
    const Rcpp::CharacterVector tz_v(tz_p);
    ConstPseudoVectorInt64 tm(tm_v);
    ConstPseudoVectorChar  tz(tz_v);
    Rcpp::IntegerVector    res(tm_v.size());

    for (R_xlen_t i=0; i<tm_v.size(); ++i) {
      const auto tz_i = Rcpp::as<std::string>(tz[i]);
      const auto tm_i = *reinterpret_cast<const Global::dtime*>(&tm[i]);
      const auto offset = getOffsetCnv(tm_i, tz_i.c_str());
      date::sys_days t_days = date::floor<date::days>(tm_i + offset);
      date::weekday wd(t_days);
      res[i] = int(date::year_month_day(t_days).year());
    }
    copyNames(tm_v, tz_v, res);    
    return res;
  } catch(std::exception &ex) {	
    forward_exception_to_r(ex);
  } catch(...) { 
    ::Rf_error("c++ exception (unknown reason)"); 
  }

  return 0;                     // not reached
}


static std::int64_t readNanotime(const char*& sp, const char* const se, const char* tzstr) {
  try {
    auto tt = Global::readDtime(sp, se);

    // check we read until the end
    if (sp != se) {
      throw std::range_error("Error parsing");
    }

    if (tt.tzstr.size() && strnlen(tzstr, Global::MAX_TZ_STR_LENGTH)) {
      throw std::range_error("timezone is specified twice: in the string and as an argument");
    }
    
    const cctz::civil_second cvt(tt.y, tt.m, tt.d, tt.hh, tt.mm, tt.ss);

    typedef Global::time_point<Global::seconds> CONVERT_TO_TIMEPOINT(const cctz::civil_second& cs, const char* tzstr);
    CONVERT_TO_TIMEPOINT *convertToTimePoint = (CONVERT_TO_TIMEPOINT*)  R_GetCCallable("RcppCCTZ", "_RcppCCTZ_convertToTimePoint" );

    auto final_tzstr = tt.tzstr.size() ? tt.tzstr.c_str() : tzstr;
    if (final_tzstr[0] == 0) {
      throw std::range_error("Error parsing");
    }
    auto tp = convertToTimePoint(cvt, final_tzstr);
    return (tp.time_since_epoch().count() - tt.offset) * 1000000000ll + tt.ns;
  } catch(std::exception &ex) {	
    forward_exception_to_r(ex);
  } catch(...) { 
    ::Rf_error("c++ exception (unknown reason)"); 
  }

  return 0;                     // not reached
}


RcppExport SEXP _nanotime_make(SEXP nt_p, SEXP tz_p) {
  checkVectorsLengths(nt_p, tz_p);
  const Rcpp::CharacterVector nt_v(nt_p);
  const Rcpp::CharacterVector tz_v(tz_p);
  ConstPseudoVectorChar nt(nt_v);
  ConstPseudoVectorChar tz(tz_v);
  Rcpp::NumericVector res(nt.size());
  for (R_xlen_t i=0; i<nt.size(); ++i) {
    auto str = (const char*)(nt[i]);
    const auto t = readNanotime(str, str + nt[i].size(), tz[i]);
    const double *ptr = reinterpret_cast<const double*>(&t);
    res[i] = *ptr;
  }
  copyNames(nt_v, tz_v, res);
  return assignS4("nanotime", res, "integer64");
}

