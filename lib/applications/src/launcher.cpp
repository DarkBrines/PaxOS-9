#include <launcher.hpp>

#include <app.hpp>

int launcher()
{
    gui::elements::Window win;

    Label *hour = new Label(86, 42, 148, 41);
    hour->setText("10:30");    // hour
    hour->setVerticalAlignment(Label::Alignement::CENTER);
    hour->setHorizontalAlignment(Label::Alignement::CENTER);
    hour->setFontSize(36);
    win.addChild(hour);
    
    Label *date = new Label(55, 89, 210, 18);
    date->setText("Lundi 25 Mars");
    date->setVerticalAlignment(Label::Alignement::CENTER);
    date->setHorizontalAlignment(Label::Alignement::CENTER);
    date->setFontSize(16);
    win.addChild(date);

    std::vector<gui::ElementBase*> apps;

    for (int i = 0; i < app::appList.size(); i++)
    {
        Box* box = new Box(60 + 119 * (i%2), 164 + 95 * int(i/2), 80, 80);
        
        std::cout << (app::appList[i].path / "../icon.png").str() << std::endl;
        Image* img = new Image(app::appList[i].path / "../icon.png", 20, 6, 40, 40);
        img->load();
        box->addChild(img);

        Label* text = new Label(0, 46, 80, 34);
        text->setText(app::appList[i].name);
        text->setVerticalAlignment(Label::Alignement::CENTER);
        text->setHorizontalAlignment(Label::Alignement::CENTER);
        text->setFontSize(16);
        box->addChild(text);

        win.addChild(box);

        apps.push_back(box);
    }

    Label* networkLabel = new Label(120, 360, 20, 20);
    networkLabel->setBackgroundColor(COLOR_DARK);
    networkLabel->setText("Network");
    networkLabel->setTextColor(COLOR_WHITE);
    networkLabel->setFontSize(16);
    win.addChild(networkLabel);

    Label* progressLabel = new Label(10, 400, 0, 20);
    progressLabel->setBackgroundColor(COLOR_SUCCESS);
    win.addChild(progressLabel);

    while (!hardware::getHomeButton())
    {
        for (int i = 0; i < apps.size(); i++)
        {
            if(apps[i]->isTouched())
            {
                return i;
            }
        }

        if (networkLabel->isTouched())
        {
            std::cout << "connection status " << network::NetworkManager::sharedInstance->isConnected() << std::endl;

            #if !defined(ESP32)
            network::URLSessionDataTask* getTask = network::URLSession::defaultInstance.get()->dataTaskWithURL(network::URL("https://www.youtube.com/s/player/652ba3a2/player_ias.vflset/fr_FR/base.js"), [](const std::string& data)
            {
                {
                    std::cout << "get request data: " << std::endl;
                }
            });
            getTask->downloadProgressHandler = [&](double progress)
            {
                std::cout << "Received progress " << progress << std::endl;
                progressLabel->setWidth(300 * progress);

                if (networkLabel->isTouched())
                {
                    if (getTask->state == network::URLSessionTask::State::Running)
                    {
                        getTask->cancel();
                        progressLabel->setWidth(0);
                    }
                    
                }

                win.updateAll();
            };
            getTask->uploadProgressHandler = [](double progress)
            {
                std::cout << "Received upload progress " << progress << std::endl;
            };

            getTask->resume();
            #endif

            network::URLRequest advancedGETRequest = network::URLRequest(network::URL("https://azerpoiu.requestcatcher.com/test"));
            advancedGETRequest.httpHeaderFields.insert(std::pair<std::string, std::string>("Custom-Header", "Hello world!"));
            network::URLSessionDataTask* advancedGetTask = network::URLSession::defaultInstance.get()->dataTaskWithRequest(advancedGETRequest, [](const std::string& data)
            {
                {
                    std::cout << "get advanced request data: " << data << std::endl;
                }
            });
            advancedGetTask->resume();

            network::URLRequest advancedPOSTRequest = network::URLRequest(network::URL("https://azerpoiu.requestcatcher.com/test"));
            advancedPOSTRequest.method = network::URLRequest::HTTPMethod::POST;
            advancedPOSTRequest.httpBody = "Hello world!";
            advancedPOSTRequest.httpHeaderFields.insert(std::pair<std::string, std::string>("Custom-Header", "Hello world!"));
            network::URLSessionDataTask* postTask = network::URLSession::defaultInstance.get()->dataTaskWithRequest(advancedPOSTRequest, [&](const std::string& data)
            {
                {
                    std::cout << "get advanced post data: " << data << std::endl;
                    networkLabel->setText(data);
                    networkLabel->setWidth(100);
                    networkLabel->setBackgroundColor(COLOR_WHITE);
                    networkLabel->setTextColor(COLOR_DARK);
                }
            });

            postTask->resume();
        }

        if (app::request)
        {
            app::request = false;
            app::runApp(app::requestingApp);
        }

        win.updateAll();
    }

    return -1;
}