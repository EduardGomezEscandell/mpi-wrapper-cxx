#include "mpicxx/mpi.h"

#include "distributed_canvas.h"
#include "settings.h"
#include "maths.h"
#include <unistd.h>

auto comm = mpi::communicator::get_default();

int main() {
    
    auto config = settings{
        std::complex<double>{-0.6, 0},
        std::complex<double>{4.0, 2.25},
        5, 20,
        20, true
    };
    config.adjust_span();

    distributed_canvas canvas(config.img_width, config.img_height, comm);
    
    logline(config, true, "Rank ", comm.rank(), " is in charge of rows ", canvas.rows().front(), " until ", canvas.rows().back());
    
    update_image(config, canvas);
}