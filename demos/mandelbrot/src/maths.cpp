#include <algorithm>
#include <execution>

#include "maths.h"

constexpr double complex_squared_norm(std::complex<double> z) {
    return z.real()*z.real() + z.imag()*z.imag();
}

inline unsigned mandelbrot_escape_time(std::complex<double> const& c, unsigned max_iter) {
    constexpr double escape_radius = 2.0;
    constexpr double escape2 = escape_radius*escape_radius;

    std::complex<double> z {0, 0};
    for(unsigned iter=0; iter < max_iter; ++iter)
    {
        // TODO: Check if complex multiplication is implemented in hardware
        double r1 = z.real()*z.real();
        double r2 = z.imag()*z.imag();
        double im = 2*z.real()*z.imag();

        const std::complex<double> z2 {r1 - r2, im};
        z = z2 + c;

        const double norm2 = r1 + r2;
        if(norm2 > escape2) {
            return iter;
        }
    }
    return max_iter;
}

void update_image(settings& config, distributed_canvas& canvas) {
    // This wouldn't be necessary if std::ranges::iota_view::iterator was a forward iterator :(
    auto rows = std::vector<std::size_t>(canvas.rows().begin(), canvas.rows().end());
    auto cols = std::vector<std::size_t>(canvas.cols().begin(), canvas.cols().end());

    std::complex<double> top_left;
    top_left.real(config.center.real() - config.span.real() / 2.0);
    top_left.imag(config.center.imag() + config.span.imag() / 2.0);
    
    std::for_each(std::execution::par_unseq, rows.begin(), rows.end(), 
      [&cols, &top_left, &config, &canvas](std::size_t row) {

        const double progress = static_cast<double>(row) / static_cast<double>(canvas.global_height());
        const double imag = top_left.imag() - progress * config.span.imag();

        std::for_each(std::execution::unseq, cols.begin(), cols.end(),
          [imag, &top_left, &config, &canvas, row] (std::size_t col) {
            const double progress = static_cast<double>(col) / static_cast<double>(canvas.global_width());
            const double real = top_left.real() + progress * config.span.real();
            
            std::complex<double> c {real, imag};
            canvas.get(row, col) = mandelbrot_escape_time(c, config.max_iter);
        });
    });
}
