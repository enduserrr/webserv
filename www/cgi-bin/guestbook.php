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

$successPage = file_get_contents("/home/asalo/Code/enduserrr/c++/webserv/www/guestbook.html");
echo $successPage;

