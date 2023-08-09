#include <iostream>
#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

Texture bolaT;

Sprite bola;
float velX = 2;
float velY = 2;



int main(){
	
	if(!bolaT.loadFromFile("Shiny_steel_ball.png")){
			cout<< "Error al cargar textura Bola"<< endl;
	}

	bola.setTexture(bolaT);
	bola.setOrigin((float)bolaT.getSize().x/2, (float)bolaT.getSize().y/2);
	bola.setPosition(425,250);


	bola.setScale(0.01f, 0.01f);

    	RenderWindow ventana(VideoMode(850,500),"Pong");
    	ventana.setFramerateLimit(120);
    	while(ventana.isOpen()){
        	Event event;
        	while (ventana.pollEvent(event)) {
            	if (event.type == Event::Closed) {
            	    ventana.close();
           	 }
        	}

		bola.move(velX,velY);

		float ballRadius = (float)bolaT.getSize().x * 0.01f / 2;

		if(bola.getPosition().x - ballRadius > 850 || bola.getPosition().x + ballRadius < 0){
			velX *= -1;
		}
		if(bola.getPosition().y - ballRadius > 500 || bola.getPosition().y + ballRadius < 0){
			velY *= -1;
		}


        	ventana.clear();
		ventana.draw(bola);
        	ventana.display();
    	}
    	return 0;
}
