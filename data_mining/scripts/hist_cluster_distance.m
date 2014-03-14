clear all;
path='H:\eclipse_workspace\atrmat\cluster_result\parse\cluster_distribute_';
files=[1 3 5 9 11 13 15 17 19 21 23 25 27 29];
%files=[1 3 5 9 11 13 15 17 19 21];
    %,'11','13','15','17','19','21','23','25','27','29'];
fileType='.csv';
N= length(files)
for i=1:1:14
    fileNum = num2str(files(i));
    filePath=[path,fileNum,fileType]
    result=importdata(filePath);
    [a,n]=hist(result);
    bar(n,a);
    xlabel('distance');
    ylabel('point num');
    %legend('cluster 1');
    title(['distribution in cluster #',fileNum]);
    saveas(gcf,[path,fileNum,'.png']);
    close all;
end
close all;