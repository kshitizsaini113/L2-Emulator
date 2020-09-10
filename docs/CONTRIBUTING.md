# Guidelines for Contibution

## Open for Contribution

1. Fork the repository and then clone it. For cloning command is:
```
$ git clone "https://github.com/<username>/TCP-IP-Stack-Functionality"
```

3. Do changes and stage them.
```
$ git add <filename>
```

4. Commit you changes with a commit message.
```
$ git commit -m "<message>"
```

5. Push changes to your forked repository
```
$ git push -u origin branchname
```
6. Create a pull request to the upstream repository.

### For creating Pull Request
Create a pull request mentioning the functionalities of the changes you have made and explain it in the comment.

## Synchronize forked repository with Main repository (Needed if creating a branch)

1. Create upstream as our repository
```
$ git remote add main "https://github.com/kshitizsaini113/TCP-IP-Stack-Functionality"
```

2. Fetch changes from main repository
```
$ git fetch main
```

3. Merge changes after being fetched
```
$ git merge main/master
```

5. Push changes to your forked repository
```
$ git push -f origin master
```

## Issue

In case you face any problem or want to suggest any changes, create issue clearly mentioning your problem.
