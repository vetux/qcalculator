/**
 *  QCalc - Extensible programming calculator
 *  Copyright (C) 2021  Julian Zampiccoli
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
//TODO: Implement various arithmetic functions for decimals

#include <string>
#include "decimal.hh"

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
        inline bool string_to_real(Iterator &itr_external, const Iterator end, decimal::Decimal &t,
                                   numeric::details::mpdecimal_type_tag);

    }

    namespace rtl {
        namespace io {
            namespace details {
                inline void print_type(const std::string &, const decimal::Decimal &v,
                                       exprtk::details::numeric::details::mpdecimal_type_tag);
            }
        }
    }

    using details::is_true;
}

#include "exprtk.hpp"

namespace exprtk {
    namespace details {
        namespace constant {
            // Used only for initialization of the constants, expression decimals use decimal::context.
            // Possible problem: Clang complains that context_template is uninitialized when constructing the context
            // but exprtk requires the constants to be initialized statically
            static decimal::Context mpdecimal_context = decimal::Context(4000,99999, -99999);

            static const decimal::Decimal e = decimal::Decimal("2.71828182845904523536028747135266249775724709369995",
                                                               mpdecimal_context);
            static const decimal::Decimal pi = decimal::Decimal("3.14159265358979323846264338327950288419716939",
                                                                mpdecimal_context);
            static const decimal::Decimal pi_2 = pi / decimal::Decimal("2.0", mpdecimal_context);
            static const decimal::Decimal pi_4 = pi / decimal::Decimal("4.0", mpdecimal_context);
            static const decimal::Decimal pi_180 = pi / decimal::Decimal("180.0", mpdecimal_context);
            static const decimal::Decimal _1_pi = decimal::Decimal("1.0") / pi;
            static const decimal::Decimal _2_pi = decimal::Decimal("2.0") / pi;
            static const decimal::Decimal _180_pi = decimal::Decimal("180.0") / pi;
            static const decimal::Decimal log2 = decimal::Decimal("2.0").ln(mpdecimal_context);
            static const decimal::Decimal sqrt2 = decimal::Decimal("2.0").sqrt(mpdecimal_context);
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
                struct epsilon_type<mpdecimal_type_tag> {
                    static inline decimal::Decimal value() {
                        static const decimal::Decimal epsilon = decimal::Decimal("1.0")
                                                                / decimal::Decimal("1e+20");
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
                inline T abs_impl(const T &v, mpdecimal_type_tag) { return v.abs(); }

                template<typename T>
                inline T acos_impl(const T &v, mpdecimal_type_tag) { throw std::runtime_error("acos not implemented"); }

                template<typename T>
                inline T acosh_impl(const T &v, mpdecimal_type_tag) {
                    throw std::runtime_error("acosh not implemented");
                }

                template<typename T>
                inline T asin_impl(const T &v, mpdecimal_type_tag) { throw std::runtime_error("asin not implemented"); }

                template<typename T>
                inline T asinh_impl(const T &v, mpdecimal_type_tag) {
                    throw std::runtime_error("asinh not implemented");
                }

                template<typename T>
                inline T atan_impl(const T &v, mpdecimal_type_tag) { throw std::runtime_error("atan not implemented"); }

                template<typename T>
                inline T atanh_impl(const T &v, mpdecimal_type_tag) {
                    throw std::runtime_error("atanh not implemented");
                }

                template<typename T>
                inline T ceil_impl(const T &v, mpdecimal_type_tag) { return v.ceil(); }

                template<typename T>
                inline T cos_impl(const T &v, mpdecimal_type_tag) { throw std::runtime_error("cos not implemented"); }

                template<typename T>
                inline T cosh_impl(const T &v, mpdecimal_type_tag) { throw std::runtime_error("cosh not implemented"); }

                template<typename T>
                inline T exp_impl(const T &v, mpdecimal_type_tag) { return v.exp(); }

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
                inline T pos_impl(const T &v, mpdecimal_type_tag) { return v; }

                template<typename T>
                inline T sin_impl(const T &v, mpdecimal_type_tag) { throw std::runtime_error("sin not implemented"); }

                template<typename T>
                inline T sinh_impl(const T &v, mpdecimal_type_tag) { throw std::runtime_error("sinh not implemented"); }

                template<typename T>
                inline T sqrt_impl(const T &v, mpdecimal_type_tag) { return v.sqrt(); }

                template<typename T>
                inline T tan_impl(const T &v, mpdecimal_type_tag) { throw std::runtime_error("tan not implemented"); }

                template<typename T>
                inline T tanh_impl(const T &v, mpdecimal_type_tag) { throw std::runtime_error("tanh not implemented"); }

                template<typename T>
                inline T cot_impl(const T &v, mpdecimal_type_tag) { throw std::runtime_error("cot not implemented"); }

                template<typename T>
                inline T sec_impl(const T &v, mpdecimal_type_tag) { throw std::runtime_error("sec not implemented"); }

                template<typename T>
                inline T csc_impl(const T &v, mpdecimal_type_tag) { throw std::runtime_error("csc not implemented"); }

                template<typename T>
                inline T r2d_impl(const T &v, mpdecimal_type_tag) { return (v * exprtk::details::constant::_180_pi); }

                template<typename T>
                inline T d2r_impl(const T &v, mpdecimal_type_tag) { return (v * exprtk::details::constant::pi_180); }

                template<typename T>
                inline T d2g_impl(const T &v, mpdecimal_type_tag) {
                    return (v * decimal::Decimal(decimal::Decimal("20.0") / decimal::Decimal("9.0")));
                }

                template<typename T>
                inline T g2d_impl(const T &v, mpdecimal_type_tag) {
                    return (v * decimal::Decimal(decimal::Decimal("9.0") / decimal::Decimal("20.0")));
                }

                template<typename T>
                inline T notl_impl(const T &v, mpdecimal_type_tag) {
                    return (v != decimal::Decimal(0) ? decimal::Decimal(0) : decimal::Decimal(1));
                }

                template<typename T>
                inline T frac_impl(const T &v, mpdecimal_type_tag) { throw std::runtime_error("Method not implemented"); }

                template<typename T>
                inline T trunc_impl(const T &v, mpdecimal_type_tag) { throw std::runtime_error("Method not implemented"); }

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
                inline T expm1_impl(const T &v, mpdecimal_type_tag) {
                    throw std::runtime_error("Method not implemented");
                }

                template<typename T>
                inline T min_impl(const T &v0, const T &v1, mpdecimal_type_tag) {
                    throw std::runtime_error("Method not implemented");
                }

                template<typename T>
                inline T max_impl(const T &v0, const T &v1, mpdecimal_type_tag) {
                    throw std::runtime_error("Method not implemented");
                }

                template<typename T>
                inline T nequal_impl(const T &v0, const T &v1, mpdecimal_type_tag) {
                    throw std::runtime_error("Method not implemented");
                }

                template<typename T>
                inline T sgn_impl(const T &v, mpdecimal_type_tag) {
                    throw std::runtime_error("Method not implemented");
                }

                template<typename T>
                inline T log1p_impl(const T &v, mpdecimal_type_tag) {
                    throw std::runtime_error("Method not implemented");
                }

                template<typename T>
                inline T erf_impl(const T &v, mpdecimal_type_tag) {
                    throw std::runtime_error("Method not implemented");
                }

                template<typename T>
                inline T erfc_impl(const T &v, mpdecimal_type_tag) {
                    throw std::runtime_error("Method not implemented");
                }

                template<typename T>
                inline T ncdf_impl(const T &v, mpdecimal_type_tag) {
                    throw std::runtime_error("Method not implemented");
                }

                template<typename T>
                inline T modulus_impl(const T &v0, const T &v1, mpdecimal_type_tag) {
                    throw std::runtime_error("Method not implemented");
                }

                template<typename T>
                inline T pow_impl(const T &v0, const T &v1, mpdecimal_type_tag) {
                    throw std::runtime_error("Method not implemented");
                }

                template<typename T>
                inline T logn_impl(const T &v0, const T &v1, mpdecimal_type_tag) {
                    throw std::runtime_error("Method not implemented");
                }

                template<typename T>
                inline T sinc_impl(const T &v, mpdecimal_type_tag) {
                    throw std::runtime_error("Method not implemented");
                }

                template<typename T>
                inline T xor_impl(const T &v0, const T &v1, mpdecimal_type_tag) {
                    throw std::runtime_error("Method not implemented");
                }

                template<typename T>
                inline T xnor_impl(const T &v0, const T &v1, mpdecimal_type_tag) {
                    throw std::runtime_error("Method not implemented");
                }

                template<typename T>
                inline T equal_impl(const T &v0, const T &v1, mpdecimal_type_tag) {
                    return decimal::Decimal(std::to_string(v0 == v1));
                }

                template<typename T>
                inline T round_impl(const T &v, mpdecimal_type_tag) {
                    throw std::runtime_error("Method not implemented");
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
                inline T root_impl(const T &v0, const T &v1, mpdecimal_type_tag) {
                    throw std::runtime_error("Method not implemented");
                }

                template<typename T>
                inline T hypot_impl(const T &v0, const T &v1, mpdecimal_type_tag) {
                    throw std::runtime_error("Method not implemented");
                }

                template<typename T>
                inline T atan2_impl(const T &v0, const T &v1, mpdecimal_type_tag) {
                    throw std::runtime_error("Method not implemented");
                }

                template<typename T>
                inline T shr_impl(const T &v0, const T &v1, mpdecimal_type_tag) {
                    throw std::runtime_error("Method not implemented");
                }

                template<typename T>
                inline T shl_impl(const T &v0, const T &v1, mpdecimal_type_tag) {
                    throw std::runtime_error("Method not implemented");
                }

                template<typename T>
                inline T and_impl(const T &v0, const T &v1, mpdecimal_type_tag) {
                    throw std::runtime_error("Method not implemented");
                }

                template<typename T>
                inline T nand_impl(const T &v0, const T &v1, mpdecimal_type_tag) {
                    throw std::runtime_error("Method not implemented");
                }

                template<typename T>
                inline T or_impl(const T &v0, const T &v1, mpdecimal_type_tag) {
                    throw std::runtime_error("Method not implemented");
                }

                template<typename T>
                inline T nor_impl(const T &v0, const T &v1, mpdecimal_type_tag) {
                    throw std::runtime_error("Method not implemented");
                }
            }
        }

        template<typename Iterator>
        inline bool string_to_real(Iterator &itr_external, const Iterator end, decimal::Decimal &t,
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
                inline void print_type(const std::string &, const decimal::Decimal &v,
                                       exprtk::details::numeric::details::mpdecimal_type_tag) {
                    printf("%s", v.format("f").c_str());
                }
            }
        }
    }
}

#endif //QCALC_EXPRTK_MPDECIMAL_ADAPTOR_HPP
