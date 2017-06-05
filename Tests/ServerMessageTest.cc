int main() {
    std::cout << "Pixel: " << std::endl;
    Pixel px(17, 1, 1, 21, 37);
    test_px(px);

    std::cout << "New game: " << std::endl;
    std::vector<std::string> players({"Michal", "ABCD", "abcd"});
    NewGame ng(34, 0, 44, 14, 88, players);
    test_ng(ng);

    std::cout << "Player eliminated" << std::endl;
    PlayerEliminated pe(10, 2, 55, 112);
    test_pe(pe);

    std::cout << "Game over" << std::endl;
    GameOver go(9, 3, 14);
    test_go(go);

    std::vector<std::shared_ptr<Event>> events({std::make_shared<Event>(px), std::make_shared<Event>(ng)});

    ServerMessage sm(10, events);



    return 42;
}