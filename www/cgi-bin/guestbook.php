<?php
// File to store comments
$filename = "comments.txt";

// Handle POST: Add new comment
$username = htmlspecialchars($_POST["username"] ?? "Anonymous");
$message = htmlspecialchars($_POST["message"] ?? "");
if ($username && $message) {
    $comment = "$username: $message\n";
    file_put_contents($filename, $comment, FILE_APPEND);
}

// Output an existing HTML page as the response
$successPage = file_get_contents("/home/asalo/Code/enduserrr/c++/webserv/www/guestbook.html");
echo $successPage;

// Handle GET: Display comments
// $comments = file_exists($filename) ? file_get_contents($filename) : "No comments yet.";

// // Output HTML
// echo "<!DOCTYPE html>";
// echo "<html lang='en'>";
// echo "<head>";
// echo "<meta charset='UTF-8'>";
// echo "<title>Guestbook</title>";
// // echo "<link rel='stylesheet' href='style.css'>";
// echo "</head>";
// echo "<body>";
// echo "<div class='container'>";
// echo "<h1>Guestbook</h1>";
// echo "<form action='/cgi-bin/guestbook.php' method='POST'>";
// echo "<label for='username'>Name:</label><br>";
// echo "<input type='text' id='username' name='username' placeholder='Your name'><br>";
// echo "<label for='message'>Message:</label><br>";
// echo "<textarea id='message' name='message' rows='2' placeholder='Your message' required></textarea><br>";
// echo "<input type='submit' value='Add Comment'>";
// echo "</form>";
// echo "<h2>Comments</h2>";
// echo "<pre>" . $comments . "</pre>";
// echo "<a href='/index.html' role='button'>Back to Home</a>";
// echo "</div>";
// echo "</body>";
// echo "</html>";
