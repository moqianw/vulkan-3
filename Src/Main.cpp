#pragma once
#include "Contaxt.hpp"

int main() {
    CT::ContaxtCreateInfo createinfo;
    createinfo.setHeght(980)
        .setWidgh(1080)
        .setWindowName("vulkanWindow");
    CT::Contaxt instance(createinfo);
    instance.Render();
    return 0;
}
