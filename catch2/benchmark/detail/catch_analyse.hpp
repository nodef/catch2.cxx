//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
// Adapted from donated nonius code.

#ifndef CATCH_ANALYSE_HPP_INCLUDED
#define CATCH_ANALYSE_HPP_INCLUDED

#include "../catch_clock.hpp"  // Adjust to relative path (@wolfram77)
#include "../catch_sample_analysis.hpp"  // Adjust to relative path (@wolfram77)


namespace Catch {
    class IConfig;

    namespace Benchmark {
        namespace Detail {
            SampleAnalysis analyse(const IConfig &cfg, FDuration* first, FDuration* last);
        } // namespace Detail
    } // namespace Benchmark
} // namespace Catch

#endif // CATCH_ANALYSE_HPP_INCLUDED



// BEGIN Amalgamated content from catch_analyse.cpp (@wolfram77)
#ifndef CATCH_ANALYSE_CPP_INCLUDED
#define CATCH_ANALYSE_CPP_INCLUDED
#ifdef CATCH2_IMPLEMENTATION
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
// Adapted from donated nonius code.

// #include "catch_analyse.hpp" // Disable self-include (@wolfram77)  // Adjust to relative path (@wolfram77)
#include "../catch_clock.hpp"  // Adjust to relative path (@wolfram77)
#include "../catch_sample_analysis.hpp"  // Adjust to relative path (@wolfram77)
#include "catch_stats.hpp"  // Adjust to relative path (@wolfram77)
#include "../../interfaces/catch_interfaces_config.hpp"  // Adjust to relative path (@wolfram77)
#include "../../internal/catch_move_and_forward.hpp"  // Adjust to relative path (@wolfram77)

#include <vector>

namespace Catch {
    namespace Benchmark {
        namespace Detail {
            SampleAnalysis analyse(const IConfig &cfg, FDuration* first, FDuration* last) {
                if (!cfg.benchmarkNoAnalysis()) {
                    std::vector<double> samples;
                    samples.reserve(static_cast<size_t>(last - first));
                    for (auto current = first; current != last; ++current) {
                        samples.push_back( current->count() );
                    }

                    auto analysis = Catch::Benchmark::Detail::analyse_samples(
                        cfg.benchmarkConfidenceInterval(),
                        cfg.benchmarkResamples(),
                        samples.data(),
                        samples.data() + samples.size() );
                    auto outliers = Catch::Benchmark::Detail::classify_outliers(
                        samples.data(), samples.data() + samples.size() );

                    auto wrap_estimate = [](Estimate<double> e) {
                        return Estimate<FDuration> {
                            FDuration(e.point),
                                FDuration(e.lower_bound),
                                FDuration(e.upper_bound),
                                e.confidence_interval,
                        };
                    };
                    std::vector<FDuration> samples2;
                    samples2.reserve(samples.size());
                    for (auto s : samples) {
                        samples2.push_back( FDuration( s ) );
                    }

                    return {
                        CATCH_MOVE(samples2),
                        wrap_estimate(analysis.mean),
                        wrap_estimate(analysis.standard_deviation),
                        outliers,
                        analysis.outlier_variance,
                    };
                } else {
                    std::vector<FDuration> samples;
                    samples.reserve(static_cast<size_t>(last - first));

                    FDuration mean = FDuration(0);
                    int i = 0;
                    for (auto it = first; it < last; ++it, ++i) {
                        samples.push_back(*it);
                        mean += *it;
                    }
                    mean /= i;

                    return SampleAnalysis{
                        CATCH_MOVE(samples),
                        Estimate<FDuration>{ mean, mean, mean, 0.0 },
                        Estimate<FDuration>{ FDuration( 0 ),
                                             FDuration( 0 ),
                                             FDuration( 0 ),
                                             0.0 },
                        OutlierClassification{},
                        0.0
                    };
                }
            }
        } // namespace Detail
    } // namespace Benchmark
} // namespace Catch
#endif // CATCH2_IMPLEMENTATION
#endif // CATCH_ANALYSE_CPP_INCLUDED
// END Amalgamated content from catch_analyse.cpp (@wolfram77)
