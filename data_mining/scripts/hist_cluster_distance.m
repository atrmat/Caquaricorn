clear all;
% path='H:\eclipse_workspace\atrmat\3-20-6\parse\cluster_distribute_';
% path='J:\2014_czh_thesis\data\Kmeans_80w\Kmeans_80w_一天流量和位置\直接kmeans\3-20\parse\cluster_distribute_';
path='J:\2014_czh_thesis\data\Kmeans_80w\Kmeans_80w_一天流量和位置\canopy+kmeans\3-19-5\parse\cluster_distribute_';
% files=[1 3 4 5 8 12 15 16 17 18 19 20 21 25];%uncontinus num
files=[14 25 2 7 11 16 19 23 24];
%files=[1 3 5 9 11 13 15 17 19 21];
    %,'11','13','15','17','19','21','23','25','27','29'];
fileType='.csv';
plotType=['.','+','o','x','*','s','d','v','^','<','>','p','h']; % 13 plot types
plotNum=length(plotType)-1;
colorType=['b','r','g','m','c','k','y']; % 8 color types,  ,'w',
colorNum=length(colorType)-1;
N=length(files);
% N=25;
legendStr='[';
for i=1:1:N
%     fileNum = num2str(files(i)); % uncontinus num
    fileNum = num2str(i); % continus num
    filePath=[path,fileNum,fileType];
    sgc_exist = exist(filePath, 'file'); 
    if sgc_exist ~= 2 %% if the file doesn't exist, continue loop
        continue;
    end
    result=importdata(filePath);
    [a,n]=hist(result,40);
    pt = mod(i,plotNum)+1;
    ct = mod(i,colorNum)+1;
%     bar(n,a);
%     if mod(i,plotNum)==0
%         pt=plotNum-2;
%     else
%         pt=mod(i,plotNum);
%     end
%     if mod(i,colorNum)==0
%         ct=colorNum-2;
%     else
%         ct=mod(i,colorNum);
%     end
    plot(n,a/sum(a),['-',colorType(ct),plotType(pt)]);
    grid on;
%     ksdensity(a); %% fitting line
    xlabel('distance');
    ylabel('probability');
    axis([0 max(n) 0 max(a)/sum(a)]);
%     if i==N
%         legendStr=[legendStr,'''',fileNum,''']'];
%     else
%         legendStr=[legendStr,'''',fileNum,''','];
%     end
    saveas(gcf,[path,fileNum,'.png']);
%     hold on;
%     title(['distribution in cluster #',fileNum]);
end
% title('pnf');
% legend('93578', '382791', '430278', '640449');
% legend(legendStr);
legend('G1','G2','G3','G4','G5','G6','G7','G8','G9','Location','BestOutside');
% saveas(gcf,[path,'all clusters','.png']);
% close all;