const char HTML_header[] = R"=====(
<html>
  <head>
      <title>Inicio de Sesion</title>
      <style>
        body {
          display: flex;
          justify-content: center;
          align-items: center;
          height: 100vh;
          margin: 0;
          flex-direction: column;
          text-align: center;
          background: linear-gradient(to bottom, #a8dadc, #457b9d);
          font-family: Arial, sans-serif;
          color: black; /* Cambié el color predeterminado del texto a negro */
        }
        .container {
          background: white;
          border-radius: 10px;
          padding: 20px;
          box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);
          max-width: 400px;
          width: 90%;
        }
        .container img {
          max-width: 100%;
          height: auto; 
          margin-bottom: 20px;
        }
        .container h1 {
          font-size: 24px;
          margin-bottom: 20px;
          color: black; 
        }
        form {
          display: flex;
          flex-direction: column;
          gap: 15px;
        }
        input[type="text"], input[type="password"] {
          width: 100%;
          padding: 10px;
          margin: 5px 0;
          border: 1px solid #ccc;
          border-radius: 5px;
        }
        input[type="submit"] {
          background: #1d3557;
          color: white;
          padding: 10px;
          border: none;
          border-radius: 5px;
          cursor: pointer;
        }
        input[type="submit"]:hover {
          background: #457b9d;
        }
        .social-buttons {
          display: flex;
          flex-direction: column;
          gap: 10px;
        }
        .social-buttons button {
          background: #e9ecef;
          border: none;
          padding: 10px;
          border-radius: 5px;
          cursor: pointer;
        }
        .social-buttons button:hover {
          background: #dee2e6;
        }
      </style>
  </head>
  <body>
    <div class="container">
      <img src="https://seeklogo.com/images/U/universidad-autonoma-de-yucatan-logo-267BB7414E-seeklogo.com.png" alt="Logo">
      <h1>Bienvenido</h1>
)=====";

const char HTML_footer[] = R"=====(
    </div>
  </body>
</html>
)=====";

const char HTML_form_header[] = R"=====(
      <form action="/save" method="POST">
        <p style="font-size: 16px; margin-bottom: 10px;">
          
        </p>
)=====";

const char HTML_form_footer[] = R"=====(
        <input type="submit" value="Guardar">
      </form>
)=====";

const char HTML_saved_link[] = R"=====(
    <div class="container">
      <p style="font-size: 18px; margin-bottom: 20px;">
        Información guardada correctamente.
      </p>
      <a href="/" style="font-size: 16px; text-decoration: none;">
        Haz clic aquí para regresar al inicio
      </a>
    </div>
)=====";
