<?php
// Get the user input from the form, default to "Guest" if not set
$username = htmlspecialchars($_POST["username"] ?? "Guest");

// Embed the name into a welcome message
$welcomeMessage = "Welcome, " . $username . "! Nice to see you here.";

// Output the response
echo "<!DOCTYPE html>";
echo "<html lang='en'>";
echo "<head>";
echo "<meta charset='UTF-8'>";
echo "<title>Welcome</title>";
echo "</head>";
echo "<body>";
echo "<div class='container'>";
echo "<h1>Welcome Message</h1>";
echo "<div class='message'>";
echo "<p>" . $welcomeMessage . "</p>";
echo "</div>";
echo "<a href='/index.html' role='button'>Back to Home</a>";
echo "</div>";
echo "</body>";
echo "</html>";
