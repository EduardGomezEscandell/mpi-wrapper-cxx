#pragma once

#include "distributed_canvas.h"

/**
 * Computes the number of iterations of 
 *        z_n+1 = z_n^2 + c
 * until the norm of z becomes greater than 2.
 * If the norm never exist this radius,
 * max_iter is returned
 */
std::size_t mandelbrot_escape_time(std::complex<double> const& c, std::size_t max_iter);

// Paints a canvas with current config
void update_image(settings& config, distributed_canvas& canvas);