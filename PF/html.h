const char HTML_header[] = R"=====(
<html>
  <head>
      <title>Form handler</title>
      <style>
            /* Definición de variables CSS */
            :root {
              --primary-color: #fff;
              --cursor: pointer;
            }

            * {
              margin: 0;
              padding: 0;
              box-sizing: border-box;
            }

            body {
              font-family: Verdana, Geneva, Tahoma, sans-serif;
              background-image: url('https://elosciloscopio.com/wp-content/uploads/2021/05/Que-debe-saber-sobre-el-microcontrolador-ESP8266-1024x518.jpg');
              background-size: cover; /* Asegura que la imagen cubra todo el fondo */
              background-position: center center; /* Centra la imagen en la pantalla */
              display: grid;
              place-items: center;
              min-height: 100vh;
              overflow: hidden;
            }

            .container {
              background-color: var(--primary-color);
              width: 700px;
              height: 550px;
              position: relative;
              display: grid;
              grid-template-columns: 1fr 1fr;
              place-items: center;
              line-height: 1.5;
              //box-shadow: 0 20px 30px rgba(0, 0, 0, 0.185);
            }

            .container::after {
              content: '';
              position: absolute;
              background-image: url('https://us.123rf.com/450wm/stevanovicigor/stevanovicigor1806/stevanovicigor180600100/105548959-lente-de-la-c%C3%A1mara-de-seguridad-acerc%C3%A1ndose-de-cerca-concepto-de-protecci%C3%B3n-de-la-propiedad.jpg?ver=6');
              width: 900px;
              height: 900px;
              border-radius: 50%;
              z-index: -1;
              opacity: 0.5;
            }

            .container-close {
              position: absolute;
              top: -15px;
              right: -15px;
              background-color: var(--primary-color);
              width: 45px;
              height: 45px;
              display: grid;
              place-items: center;
              font-size: 1.7rem;
              color: #2E4052;
              border-radius: 50%;
              box-shadow: 0 10px 20px rgba(0, 0, 0, 0.164);
              cursor: var(--cursor);
              display: none;
            }

            .container img {
              width: 250px;
              height: 400px;
              object-fit: cover;
              object-position: center;
            }

            .container-text {
              padding: 10px 40px 10px 10px;
            }

            .container-text h2 {
              text-align: center;
              font-size: 24px;
              color: #1A2250;
            }

            .container-text p {
              text-align: center;
              font-size: 18px;
              color: #3B4169;
              margin: 10px 0;
            }

            .container-text input,
            .container-text form {
              width: 100%;
              border: none;
              padding: 14px;
              border-radius: 3px;
            }

            .container-text input {
              border: 2px solid #DADDEC;
              margin: 5px 0 10px;
              font-size: 1rem;
              color: #656880;
            }

            .container-text form {
              background-image: linear-gradient(to left, #fca311, #000000);
              display: block;
              color: var(--primary-color);
              font-size: 1rem;
              cursor: var(--cursor);
              box-shadow: 0 5px 20px #89caff94;
              transition: box-shadow 0.3s ease-in-out;
            }

            .container-text form:hover {
              box-shadow: none;
            }

            .container-text span {
              display: block;
              text-align: center;
              margin: 20px 0 0;
              color: #BABDCB;
              font-size: 16px;
            }
            /* Responsividad */
            @media (max-width: 768px) {
              .container {
                padding: 10px; /* Reducir el espacio en dispositivos pequeños */
              }
            
              h1 {
                font-size: 1.5rem; /* Ajustar tamaño de texto */
              }
            
              p {
                font-size: 0.9rem;
              }
            
              .container-close {
                width: 35px;
                height: 35px;
                top: -10px; /* Ajustar posición */
                right: -10px;
              }
            }
            
            @media (max-width: 480px) {
              .container {
                max-width: 100%; /* Usar todo el ancho disponible */
                padding: 5px;
              }
            
              h1 {
                font-size: 1.2rem; /* Texto más pequeño en pantallas muy estrechas */
              }
            
              p {
                font-size: 0.8rem;
              }
            }
      </style>
  </head>
<body>
<div class="container">
    <div class="container-close">&times;</div>
    <img
      src="https://upload.wikimedia.org/wikipedia/commons/thumb/3/30/Logo_UADY.svg/146px-Logo_UADY.svg.png"
      alt="image">
    <div class="container-text">
)=====";

const char HTML_footer[] = R"=====(
</body>
</html>
)=====";

const char HTML_form_header[] = R"=====(
   <h2>
      <i class="far fa-thumbs-up"></i>
      Proyecto Final
   </h2>
   <p>
      Julian Adrian Viana Palomo
   </p>
  <form action="/save" method="POST">
)=====";

const char HTML_form_footer[] = R"=====(
    <input type="submit" value="Save">
    <span>Ingrear la Key Id del usuario</span>
  </form>
  </div>
  </div>
  </div>
)=====";

const char HTML_saved_link[] = R"=====(
  <a href="/" >Information saved - click here to return to the homepage</a>
)=====";
