<?php
// File to store comments
$filename = "comments.txt";

// Handle GET: Display comments
$comments = file_exists($filename) ? file_get_contents($filename) : "No comments yet.";

// Output HTML
echo "<!DOCTYPE html>";
echo "<html lang='en'>";
echo "<head>";
echo "<meta charset='UTF-8'>";
echo "<title>Guestbook - View Comments</title>";
echo "<link rel='stylesheet' href='/style.css'>";
echo "</head>";
echo "<body>";
echo "<div class='container'>";
echo "<h1>Guestbook Comments</h1>";
echo "<pre>" . $comments . "</pre>";
echo "<div class='button-container'>";
echo "<a href='/index.html' role='button'>Home</a>";
echo "<a href='/guestbook.html' role='button'>Add Comment</a>";
echo "</div>";
echo "</div>";
echo "</body>";
echo "</html>";
