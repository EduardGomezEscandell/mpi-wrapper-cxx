#include <filesystem>
#include <fstream>

#include "mpicxx/mpicxx.h"

#include "distributed_canvas.h"
#include "settings.h"
#include "maths.h"
#include "fileIO.h"


auto comm = mpi::communicator::get_default();

int main(int argc, char** argv) {
    settings config;
    if(argc == 2) {
        config = ini_reader(std::filesystem::path{argv[1]}).read();
    }

    if(comm.rank() == 0) {
        logline(config, true, config);
    }

    distributed_canvas canvas(config.img_width, config.img_height, comm);
    logline(config, true, "Rank ", comm.rank(), " is in charge of rows ", canvas.rows().front(), " until ", canvas.rows().back());
    
    update_image(config, canvas);
    comm.barrier();
    netbpm_writer{canvas, config}.write();
}