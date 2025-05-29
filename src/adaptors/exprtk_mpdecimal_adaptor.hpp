/**
 *  QCalc - Extensible programming calculator
 *  Copyright (C) 2023  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef QCALC_EXPRTK_MPDECIMAL_ADAPTOR_HPP
#define QCALC_EXPRTK_MPDECIMAL_ADAPTOR_HPP

//TODO: Generate constants
//TODO: Implement various arithmetic functions for decimals without falling back to double

#include <string>
#include <sstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>

#include <decimal.hh>

using namespace std;

namespace exprtk {
    namespace details {
        namespace numeric {
            namespace details {
                struct mpdecimal_type_tag;

                template<typename T>
                inline T const_pi_impl(mpdecimal_type_tag);

                template<typename T>
                inline T const_e_impl(mpdecimal_type_tag);
            }
        }

        inline bool is_true(const decimal::Decimal &v);

        inline bool is_false(const decimal::Decimal &v);

        template<typename Iterator>
        inline bool string_to_real(Iterator &itr_external,
                                   const Iterator end,
                                   decimal::Decimal &t,
                                   numeric::details::mpdecimal_type_tag);

    }

    namespace rtl {
        namespace io {
            namespace details {
                inline void print_type(const std::string &,
                                       const decimal::Decimal &v,
                                       exprtk::details::numeric::details::mpdecimal_type_tag);
            }
        }
    }

    using details::is_true;
}

#include "exprtk.hpp"

static std::string convertDoubleToString(const long double &value) {
    // Avoid platform-dependent NaN and Infinity string representation
    if (std::isnan(value)) {
        return "NaN";
    } else if (std::isinf(value)) {
        return "Inf";
    } else {
        std::stringstream ss;
        ss << setprecision(100) << value;
        string str;
        ss >> str;
        return str;
    }
}

namespace exprtk {
    namespace details {
        namespace constant {
            // Used only for initialization of the constants, expression decimals use decimal::context.
            // Possible problem: Clang complains that context_template is uninitialized when constructing the context
            // but exprtk requires the constants to be initialized statically
            static decimal::Context mpdecimal_context = decimal::Context(100, 999, -999);

            static const decimal::Decimal e = decimal::Decimal("2.71828182845904523536028747135266249775724709369995",
                                                               mpdecimal_context);
            static const decimal::Decimal pi = decimal::Decimal("3.14159265358979323846264338327950288419716939",
                                                                mpdecimal_context);
            static const decimal::Decimal pi_2 = pi / decimal::Decimal("2.0", mpdecimal_context);
            static const decimal::Decimal pi_4 = pi / decimal::Decimal("4.0", mpdecimal_context);
            static const decimal::Decimal pi_180 = pi / decimal::Decimal("180.0", mpdecimal_context);
            static const decimal::Decimal _1_pi = decimal::Decimal("1.0", mpdecimal_context) / pi;
            static const decimal::Decimal _2_pi = decimal::Decimal("2.0", mpdecimal_context) / pi;
            static const decimal::Decimal _180_pi = decimal::Decimal("180.0", mpdecimal_context) / pi;
            static const decimal::Decimal log2 = decimal::Decimal("2.0", mpdecimal_context).ln(mpdecimal_context);
            static const decimal::Decimal sqrt2 = decimal::Decimal("2.0", mpdecimal_context).sqrt(mpdecimal_context);
        }

        namespace numeric {
            namespace details {
                struct mpdecimal_type_tag {
                };

                template<>
                struct number_type<decimal::Decimal> {
                    typedef mpdecimal_type_tag type;
                };

                template<>
                struct epsilon_type<decimal::Decimal> {
                    static inline decimal::Decimal value() {
                        // Epsilon value is cached by exprtk after the first invocation of this method,
                        // which means the wrong epsilon value is returned when the user changes the precision.
                        decimal::Decimal epsilon = decimal::Decimal("1e-"
                                                                          + std::to_string(decimal::context.prec()),
                                                                          decimal::context);
                        return epsilon;
                    }
                };

                inline bool is_nan_impl(const decimal::Decimal &v, mpdecimal_type_tag) {
                    return v.isnan();
                }

                template<typename T>
                inline int to_int32_impl(const T &v, mpdecimal_type_tag) {
                    return static_cast<int>(v.i32());
                }

                template<typename T>
                inline long long to_int64_impl(const T &v, mpdecimal_type_tag) {
                    return static_cast<long long int>(v.i64());
                }

                template<typename T>
                inline long long to_uint64_impl(const T &v, mpdecimal_type_tag) {
                    return static_cast<long long int>(v.u64());
                }

                template<typename T>
                inline T abs_impl(const T &v, mpdecimal_type_tag) { return v.abs(); }

                template<typename T>
                inline T acos_impl(const T &v, mpdecimal_type_tag) {
                    decimal::context.add_status(MPD_Inexact);
                    return T(convertDoubleToString(acosl(std::stold(v.format("f")))));
                }

                template<typename T>
                inline T acosh_impl(const T &v, mpdecimal_type_tag) {
                    decimal::context.add_status(MPD_Inexact);
                    return T(convertDoubleToString(acoshl(std::stold(v.format("f")))));
                }

                template<typename T>
                inline T asin_impl(const T &v, mpdecimal_type_tag) {
                    decimal::context.add_status(MPD_Inexact);
                    return T(convertDoubleToString(asinl(std::stold(v.format("f")))));
                }

                template<typename T>
                inline T asinh_impl(const T &v, mpdecimal_type_tag) {
                    decimal::context.add_status(MPD_Inexact);
                    return T(convertDoubleToString(asinhl(std::stold(v.format("f")))));
                }

                template<typename T>
                inline T atan_impl(const T &v, mpdecimal_type_tag) {
                    decimal::context.add_status(MPD_Inexact);
                    return T(convertDoubleToString(atanl(std::stold(v.format("f")))));
                }

                template<typename T>
                inline T atanh_impl(const T &v, mpdecimal_type_tag) {
                    decimal::context.add_status(MPD_Inexact);
                    return T(convertDoubleToString(atanhl(std::stold(v.format("f")))));
                }

                template<typename T>
                inline T ceil_impl(const T &v, mpdecimal_type_tag) { return v.ceil(); }

                template<typename T>
                inline T cos_impl(const T &v, mpdecimal_type_tag) {
                    decimal::context.add_status(MPD_Inexact);
                    return T(convertDoubleToString(cosl(std::stold(v.format("f")))));
                }

                template<typename T>
                inline T cosh_impl(const T &v, mpdecimal_type_tag) {
                    decimal::context.add_status(MPD_Inexact);
                    return T(convertDoubleToString(coshl(std::stold(v.format("f")))));
                }

                template<typename T>
                inline T exp_impl(const T &v, mpdecimal_type_tag) {
                    decimal::context.add_status(MPD_Inexact);
                    return T(convertDoubleToString(expl(std::stold(v.format("f")))));
                }

                template<typename T>
                inline T floor_impl(const T &v, mpdecimal_type_tag) { return v.floor(); }

                template<typename T>
                inline T log_impl(const T &v, mpdecimal_type_tag) { return v.ln(); }

                template<typename T>
                inline T log10_impl(const T &v, mpdecimal_type_tag) { return v.log10(); }

                template<typename T>
                inline T log2_impl(const T &v, mpdecimal_type_tag) { return v.logb(); }

                template<typename T>
                inline T neg_impl(const T &v, mpdecimal_type_tag) { return -v; }

                template<typename T>
                inline T pos_impl(const T &v, mpdecimal_type_tag) { return +v; }

                template<typename T>
                inline T sin_impl(const T &v, mpdecimal_type_tag) {
                    decimal::context.add_status(MPD_Inexact);
                    return T(convertDoubleToString(sinl(std::stold(v.format("f")))));
                }

                template<typename T>
                inline T sinh_impl(const T &v, mpdecimal_type_tag) {
                    decimal::context.add_status(MPD_Inexact);
                    return T(convertDoubleToString(sinhl(std::stold(v.format("f")))));
                }

                template<typename T>
                inline T sqrt_impl(const T &v, mpdecimal_type_tag) { return v.sqrt(); }

                template<typename T>
                inline T tan_impl(const T &v, mpdecimal_type_tag) {
                    decimal::context.add_status(MPD_Inexact);
                    return T(convertDoubleToString(tanl(std::stold(v.format("f")))));
                }

                template<typename T>
                inline T tanh_impl(const T &v, mpdecimal_type_tag) {
                    decimal::context.add_status(MPD_Inexact);
                    return T(convertDoubleToString(tanhl(std::stold(v.format("f")))));
                }

                template<typename T>
                inline T cot_impl(const T &v, mpdecimal_type_tag) {
                    decimal::context.add_status(MPD_Inexact);
                    auto x = std::stold(v.format("f"));
                    return T(convertDoubleToString(cosl(x) / std::sin(x)));
                }

                template<typename T>
                inline T sec_impl(const T &v, mpdecimal_type_tag) {
                    decimal::context.add_status(MPD_Inexact);
                    auto x = std::stold(v.format("f"));
                    return T(convertDoubleToString(1 / cosl(x)));
                }

                template<typename T>
                inline T csc_impl(const T &v, mpdecimal_type_tag) {
                    decimal::context.add_status(MPD_Inexact);
                    auto x = std::stold(v.format("f"));
                    return T(convertDoubleToString(1 / sinl(x)));
                }

                template<typename T>
                inline T r2d_impl(const T &v, mpdecimal_type_tag) { return (v * exprtk::details::constant::_180_pi); }

                template<typename T>
                inline T d2r_impl(const T &v, mpdecimal_type_tag) { return (v * exprtk::details::constant::pi_180); }

                template<typename T>
                inline T d2g_impl(const T &v, mpdecimal_type_tag) {
                    return (v * T(T("20.0") / T("9.0")));
                }

                template<typename T>
                inline T g2d_impl(const T &v, mpdecimal_type_tag) {
                    return (v * T(T("9.0") / T("20.0")));
                }

                template<typename T>
                inline T notl_impl(const T &v, mpdecimal_type_tag) {
                    return (v != T(0) ? T(0) : T(1));
                }

                template<typename T>
                inline T frac_impl(const T &v, mpdecimal_type_tag) {
                    auto str = v.format("f");
                    auto index = str.find(".");
                    if (index == std::string::npos) {
                        return 0;
                    } else {
                        return T("0" + str.substr(index));
                    }
                }

                template<typename T>
                inline T trunc_impl(const T &v, mpdecimal_type_tag) {
                    auto str = v.format("f");
                    auto index = str.find(".");
                    if (index == std::string::npos) {
                        return v;
                    } else {
                        return T(str.substr(0, index));
                    }
                }

                template<typename T>
                inline T const_pi_impl(mpdecimal_type_tag) {
                    return exprtk::details::constant::pi;
                }

                template<typename T>
                inline T const_e_impl(mpdecimal_type_tag) {
                    return exprtk::details::constant::e;
                }

                inline bool is_true_impl(const decimal::Decimal &v) {
                    return v.i32();
                }

                inline bool is_false_impl(const decimal::Decimal &v) {
                    return !is_true_impl(v);
                }

                template<typename T>
                inline T expm1_impl(const T &v, mpdecimal_type_tag tag) {
                    return exp_impl(v, tag) - T("1");
                }

                template<typename T>
                inline T min_impl(const T &v0, const T &v1, mpdecimal_type_tag) {
                    if (v0 < v1) {
                        return v0;
                    } else {
                        return v1;
                    }
                }

                template<typename T>
                inline T max_impl(const T &v0, const T &v1, mpdecimal_type_tag) {
                    if (v0 > v1) {
                        return v0;
                    } else {
                        return v1;
                    }
                }

                template<typename T>
                inline T nequal_impl(const T &v0, const T &v1, mpdecimal_type_tag) {
                    return (v0 != v1) ? T(1) : T(0);
                }

                template<typename T>
                inline T sgn_impl(const T &v, mpdecimal_type_tag) {
                    auto zero = T("0");
                    if (v < zero) {
                        return T("-1");
                    } else if (v > zero) {
                        return T("+1");
                    } else {
                        return zero;
                    }
                }

                template<typename T>
                inline T log1p_impl(const T &v, mpdecimal_type_tag tag) {
                    return exp(log_impl(v, tag));
                }

                template<typename T>
                inline T erf_impl(const T &v, mpdecimal_type_tag) {
                    throw std::runtime_error("erf not implemented");
                }

                template<typename T>
                inline T erfc_impl(const T &v, mpdecimal_type_tag) {
                    throw std::runtime_error("erfc not implemented");
                }

                template<typename T>
                inline T ncdf_impl(const T &v, mpdecimal_type_tag) {
                    throw std::runtime_error("ncdf not implemented");
                }

                template<typename T>
                inline T modulus_impl(const T &v0, const T &v1, mpdecimal_type_tag) {
                    return v0 - (v1 * (v0 / v1).to_integral_exact());
                }

                template<typename T>
                inline T pow_impl(const T &v0, const T &v1, mpdecimal_type_tag) {
                    return v0.pow(v1);
                }

                template<typename T>
                inline T logn_impl(const T &v0, const T &v1, mpdecimal_type_tag tag) {
                    return log_impl(v0, tag) / log_impl(v1, tag);
                }

                template<typename T>
                inline T sinc_impl(const T &v, mpdecimal_type_tag tag) {
                    if (abs_impl(v, tag) >= epsilon_type<T>::value())
                        return (sin_impl(v, tag) / v);
                    else
                        return T("1");
                }

                template<typename T>
                inline T xor_impl(const T &v0, const T &v1, mpdecimal_type_tag) {
                    return (is_false_impl(v0) != is_false_impl(v1)) ? T("1") : T("0");
                }

                template<typename T>
                inline T xnor_impl(const T &v0, const T &v1, mpdecimal_type_tag) {
                    const bool v0_true = is_true_impl(v0);
                    const bool v1_true = is_true_impl(v1);
                    if ((v0_true && v1_true) || (!v0_true && !v1_true))
                        return T(1);
                    else
                        return T(0);
                }

                template<typename T>
                inline T equal_impl(const T &v0, const T &v1, mpdecimal_type_tag) {
                    return T(convertDoubleToString(v0 == v1));
                }

                template<typename T>
                inline T round_impl(const T &v, mpdecimal_type_tag) {
                    return v.to_integral_exact();
                }

                template<typename T>
                inline T roundn_impl(const T &v0, const T &v1, mpdecimal_type_tag) {
                    const T p10 = T(T("10")).pow(v1.floor());
                    if (v0 < T(0))
                        return T(T((v0 * p10) - T("0.5")).ceil() / p10);
                    else
                        return T(T((v0 * p10) + T("0.5")).floor() / p10);
                }

                template<typename T>
                inline bool is_integer_impl(const T &v, mpdecimal_type_tag) {
                    return v.isinteger();
                }

                template<typename T>
                inline T root_impl(const T &v0, const T &v1, mpdecimal_type_tag tag) {
                    return pow_impl(v0, T("1") / v1, tag);
                }

                template<typename T>
                inline T hypot_impl(const T &v0, const T &v1, mpdecimal_type_tag tag) {
                    return sqrt_impl(v0 * v0 + v1 * v1, tag);
                }

                template<typename T>
                inline T atan2_impl(const T &v0, const T &v1, mpdecimal_type_tag) {
                    decimal::context.add_status(MPD_Inexact);
                    return T(convertDoubleToString(atan2l(std::stold(v0.format("f")),
                                                          std::stold(v1.format("f")))));
                }

                template<typename T>
                inline T shr_impl(const T &v0, const T &v1, mpdecimal_type_tag tag) {
                    return v0 * (T("1") / pow_impl(T("2.0"), v1, tag));
                }

                template<typename T>
                inline T shl_impl(const T &v0, const T &v1, mpdecimal_type_tag tag) {
                    return v0 * pow_impl(T("2.0"), v1, tag);
                }

                template<typename T>
                inline T and_impl(const T &v0, const T &v1, mpdecimal_type_tag) {
                    return (is_true_impl(v0) && is_true_impl(v1)) ? T("1") : T("0");
                }

                template<typename T>
                inline T nand_impl(const T &v0, const T &v1, mpdecimal_type_tag) {
                    return (is_false_impl(v0) || is_false_impl(v1)) ? T("1") : T("0");
                }

                template<typename T>
                inline T or_impl(const T &v0, const T &v1, mpdecimal_type_tag) {
                    return (is_true_impl(v0) || is_true_impl(v1)) ? T("1") : T("0");
                }

                template<typename T>
                inline T nor_impl(const T &v0, const T &v1, mpdecimal_type_tag) {
                    return (is_false_impl(v0) && is_false_impl(v1)) ? T("1") : T("0");
                }
            }
        }

        template<typename Iterator>
        inline bool string_to_real(Iterator &itr_external,
                                   const Iterator end, decimal::Decimal &t,
                                   numeric::details::mpdecimal_type_tag) {
            t = decimal::Decimal(std::string(itr_external, end));
            return true;
        }

        inline bool is_true(const decimal::Decimal &v) { return details::numeric::details::is_true_impl(v); }

        inline bool is_false(const decimal::Decimal &v) { return details::numeric::details::is_false_impl(v); }
    }

    namespace rtl {
        namespace io {
            namespace details {
                inline void print_type(const std::string &,
                                       const decimal::Decimal &v,
                                       exprtk::details::numeric::details::mpdecimal_type_tag) {
                    printf("%s", v.format("f").c_str());
                }
            }
        }
    }
}

#endif //QCALC_EXPRTK_MPDECIMAL_ADAPTOR_HPP
